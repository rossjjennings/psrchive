/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/CalibrationInterpolator.h"
#include "Pulsar/CalibrationInterpolatorExtension.h"

#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/FluxCalibratorExtension.h"

#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/PolnCalibratorExtension.h"

#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/Integration.h"

#ifdef HAVE_SPLINTER
#include "Pulsar/SplineSmooth.h"
#endif

// #define _DEBUG 1
#include "debug.h"

using namespace std;
using namespace Pulsar;

CalibrationInterpolator::CalibrationInterpolator (Calibrator* cal)
{
  // This class works by inserting new extensions into the archive
  construct ( const_cast<Archive*>( cal->get_Archive() ) );
}

CalibrationInterpolator::CalibrationInterpolator (Archive* archive)
{
  construct ( archive );
}

void CalibrationInterpolator::construct (Archive* data)
{
  interpolator = data->get<CalibrationInterpolatorExtension> ();
  if (!interpolator)
    throw Error (InvalidParam, "CalibrationInterpolator ctor",
		 data->get_filename()
		 + " does not contain a CalibrationInterpolatorExtension");

  bool has_feedpar = false;  // has feed parameters
  bool has_calpoln = false;  // has calibrator stokes parameters
  bool has_fluxcal = false;  // has flux calibrator parameters
  
  unsigned nparam = interpolator->get_nparam();

  DEBUG("CalibrationInterpolator constructing splines");
  
  for (unsigned iparam=0; iparam < nparam; iparam++)
  {
    auto param = interpolator->get_parameter (iparam);
    switch (param->get_code())
    {
    case CalibrationInterpolatorExtension::Parameter::FrontendParameter:
    {
      has_feedpar = true;
      feedpar_splines[ param->iparam ]
	= new SplineSmooth2D (param->interpolator);
      break;
    }
    
    case CalibrationInterpolatorExtension::Parameter::CalibratorStokesParameter:
    {
      has_calpoln = true;
      calpoln_splines[ param->iparam ]
	= new SplineSmooth2D (param->interpolator);
      break;
    }

    case CalibrationInterpolatorExtension::Parameter::FluxCalibratorParameter:
    {
      has_fluxcal = true;
      fluxcal_splines[ param->iparam ]
	= new SplineSmooth2D (param->interpolator);
      break;
    }
    }
  }

  if (has_fluxcal)
  {
    fluxcal = data->getadd<FluxCalibratorExtension> ();
    fluxcal->set_nreceptor( interpolator->get_nreceptor() );
    fluxcal->has_scale( interpolator->get_native_scale() );
  }
    
  if (has_feedpar)
  {
    feedpar = data->getadd<PolnCalibratorExtension> ();
    feedpar->set_type (interpolator->get_type());
    feedpar->set_epoch (interpolator->get_reference_epoch());
  }
  
  if (has_calpoln)
  {
    calpoln = data->getadd<CalibratorStokes> ();
    calpoln->set_coupling_point (interpolator->get_coupling_point());
  }
}


//! Destructor
CalibrationInterpolator::~CalibrationInterpolator ()
{
}

//! Get the extension from which this object was constructed
const CalibrationInterpolatorExtension* 
CalibrationInterpolator::get_extension()
{
  return interpolator;
}

//! Get the type of the calibrator
const Calibrator::Type* CalibrationInterpolator::get_type () const
{
  return interpolator->get_type ();
}

template<typename C>
void set_params (C* container,
		 std::map< unsigned, Reference::To<SplineSmooth2D> >& params,
		 unsigned ichan,
		 double x, double y)
{
  std::pair<double,double> coord (x,y);

  // cerr << "set_params size=" << params.size() << endl;
  
  for (auto param: params)
  {
    unsigned iparam = param.first;
    DEBUG("iparam=" << iparam << " nparam=" << container->get_nparam());

    if (iparam >= container->get_nparam())
      throw Error (InvalidParam, "CalibrationInterpolator::set_params",
		   "iparam=%u nparam%u", iparam, container->get_nparam());

    SplineSmooth2D* spline = param.second;
    double value = spline->evaluate (coord);

    // To-Do: fit 2-D splines to bootstrap (replacement) errors
    Estimate<float> estimate (value, 0.001);
    
    container->set_Estimate (iparam, ichan, estimate);
  }
}

bool CalibrationInterpolator::update (const MJD& epoch, 
                                      const vector<double>& frequency)
{
  DEBUG("CalibrationInterpolator::update");
  
  /*
    update the feedpar and calpoln attributes
  */

  double expiry_days = 1.0;

  double interval = (epoch - last_computed).in_days();
  if (fabs (interval) < expiry_days)
  {
    DEBUG("CalibrationInterpolator::update no need to update");
    return false;
  }

  DEBUG("CalibrationInterpolator::update compute");

  MJD min_epoch = interpolator->get_minimum_epoch ();
  MJD max_epoch = interpolator->get_maximum_epoch ();
 
  double before = (min_epoch - epoch).in_days();
  double after = (epoch - max_epoch).in_days(); 
  if (before > expiry_days || after > expiry_days)
    throw Error (InvalidParam, "CalibrationInterpolator::update",
		 "requested epoch=" + epoch.printdays(6) +
		 " not spanned (" + min_epoch.printdays(6) +
		 " - " + max_epoch.printdays(6) +")" );
      
  double x0 = (epoch - interpolator->get_reference_epoch ()).in_days();
  unsigned nchan = frequency.size();
  
  if (feedpar)
  {
    DEBUG("CalibrationInterpolator::update PolnCalibrator::nchan="<<nchan);
    feedpar->set_nchan (nchan);
  }
  
  if (calpoln)
  {
    DEBUG("CalibrationInterpolator::update CalibratorStokes::nchan="<<nchan);
    calpoln->set_nchan (nchan);
  }
  
  if (fluxcal)
  {
    DEBUG("CalibrationInterpolator::update FluxCalibrator::nchan="<<nchan);
    fluxcal->set_nchan (nchan);
    // complete the resize
    fluxcal->set_nreceptor (interpolator->get_nreceptor());
  }
  
  double min_freq = interpolator->get_minimum_frequency ();
  double max_freq = interpolator->get_maximum_frequency ();
  double ref_freq = interpolator->get_reference_frequency ();

  if (feedpar)
  {
    // set the gain to unity
    unsigned gain_iparam = 0;
    for (unsigned ichan=0; ichan<nchan; ++ichan)
      feedpar->set_Estimate (gain_iparam, ichan, Estimate<float>(1.0,1e-4));
  }

  for (unsigned ichan=0; ichan<nchan; ++ichan) try
  {
    double freq = frequency[ichan];
    bool valid = (freq >= min_freq && freq <= max_freq);

    DEBUG("CalibrationInterpolator::update ichan=" << ichan << " freq=" << freq << " valid=" << valid);

    if (feedpar)
    {
      feedpar->set_valid (ichan, valid);
      if (valid)
	set_params (feedpar.get(), feedpar_splines, ichan, x0, freq - ref_freq);
    }
    
    if (calpoln)
    {
      calpoln->set_valid (ichan, valid);
      if (valid)
	set_params (calpoln.get(), calpoln_splines, ichan, x0, freq - ref_freq);
    }

    if (fluxcal)
    {
      DEBUG("CalibrationInterpolator::update fluxcal ichan=" << ichan << " freq=" << freq << " valid=" << valid);
      fluxcal->set_valid (ichan, valid);
      if (valid)
	set_params (fluxcal.get(), fluxcal_splines, ichan, x0, freq - ref_freq);
    }

  }
  catch (Error& error)
  {
    if (Archive::verbose > 1)
      cerr << "CalibrationInterpolator::update"
	" error ichan=" << ichan << error << endl;
  }

  if (Archive::verbose > 2)
    cerr << "CalibrationInterpolator::update exit" << endl;

  last_computed = epoch;

  return true;
}

