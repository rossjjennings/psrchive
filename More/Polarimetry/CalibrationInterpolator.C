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

#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/PolnCalibratorExtension.h"

#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/Integration.h"

#ifdef HAVE_SPLINTER
#include "Pulsar/SplineSmooth.h"
#endif

using namespace std;
using namespace Pulsar;

CalibrationInterpolator::CalibrationInterpolator (PolnCalibrator* cal)
{
  // This class works by inserting new extensions into the archive
  Reference::To<Archive> data = const_cast<Archive*>( cal->get_Archive() );
  
  interpolator = data->get<CalibrationInterpolatorExtension> ();
  if (!interpolator)
    throw Error (InvalidParam, "CalibrationInterpolator ctor",
		 data->get_filename()
		 + " does not contain a CalibrationInterpolatorExtension");

  bool has_feedpar = false;  // has feed parameters
  bool has_calpoln = false;   // has calibrator stokes parameters
  
  unsigned nparam = interpolator->get_nparam();

  cerr << "CalibrationInterpolator constructing splines" << endl;
  
  for (unsigned iparam=0; iparam < nparam; iparam++)
  {
    auto param = interpolator->get_parameter (iparam);
    if (param->get_code() == "FEEDPAR")
    {
      has_feedpar = true;
      feedpar_splines[ param->iparam ]
	= new SplineSmooth2D (param->interpolator);
    }
    
    else if (param->get_code() == "CAL_POLN")
    {
      has_calpoln = true;
      calpoln_splines[ param->iparam ]
	= new SplineSmooth2D (param->interpolator);
    }
  }

  if (!has_feedpar)
    throw Error (InvalidParam, "CalibrationInterpolator ctor",
		 "CalibrationInterpolatorExtension has no feed parameters");

  feedpar = data->getadd<PolnCalibratorExtension> ();
  feedpar->set_type (interpolator->get_type());
  feedpar->set_epoch (interpolator->get_reference_epoch());
  
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

template<typename C>
void set_params (C* container,
	  std::map< unsigned, Reference::To<SplineSmooth2D> >& params,
	  double x, double y)
{
  std::pair<double,double> coord (x,y);
  
  for (auto param: params)
  {
    if (param.first >= container->get_nparam())
      throw Error (InvalidParam, "CalibrationInterpolator::set_params",
		   "iparam=%u nparam%u", param.first, container->get_nparam());

    double value = param.second->evaluate (coord);
  }
}
	  
bool CalibrationInterpolator::update (const Integration* subint)
{
  unsigned nchan = subint->get_nchan();

  /*
    update the feedpar and calpoln attributes
  */

  MJD epoch = subint->get_epoch ();

  MJD min_epoch = interpolator->get_minimum_epoch ();
  MJD max_epoch = interpolator->get_maximum_epoch ();
  
  if (epoch < min_epoch || epoch > max_epoch)
    throw Error (InvalidParam, "CalibrationInterpolator::update",
		 "sub-integration epoch=" + epoch.printdays(6) +
		 " not spanned (" + min_epoch.printdays(6) +
		 " - " + max_epoch.printdays(6) +")" );
      
  double x0 = (epoch - interpolator->get_reference_epoch ()).in_days();

  if (feedpar)
    feedpar->set_nchan (nchan);

  if (calpoln)
    calpoln->set_nchan (nchan);

  double min_freq = interpolator->get_minimum_frequency ();
  double max_freq = interpolator->get_maximum_frequency ();
  double ref_freq = interpolator->get_reference_frequency ();

  for (unsigned ichan=0; ichan<nchan; ++ichan) try
  {
    if (Archive::verbose > 2)
      cerr << "CalibrationInterpolator::update"
	" ichan=" << ichan << endl;

    double freq = subint->get_centre_frequency (ichan);
    bool valid = (freq >= min_freq && freq <= max_freq);
    
    if (feedpar)
    {
      feedpar->set_valid (ichan, valid);
      if (valid)
	set_params (feedpar.get(), feedpar_splines, x0, freq - ref_freq);
    }
    
    if (calpoln)
    {
      calpoln->set_valid (ichan, valid);
      if (valid)
	set_params (calpoln.get(), calpoln_splines, x0, freq - ref_freq);
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

  return true;
}

