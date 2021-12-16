/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CalibratorInfo.h"

#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/CalibratorStokesInfo.h"
#include "Pulsar/SolverInfo.h"
#include "Pulsar/IXRInfo.h"
#include "Pulsar/ConstantGainInfo.h"

#include "Pulsar/Archive.h"

// #define _DEBUG 1
#include "debug.h"

using namespace std;

Pulsar::CalibratorInfo::CalibratorInfo ()
{
  // reserve 5% of the viewport height for space between panels
  between_panels = 0.05;

  calibrator_stokes = false;
  calibrator_stokes_degree = false;
  reduced_chisq = false;
  intrinsic_crosspol_ratio = false;
  constant_gain = false;

  outlier_threshold = 0.0;
  subint.set_integrate( true );
}

void Pulsar::CalibratorInfo::prepare (const Archive* data)
{
  DEBUG("Pulsar::CalibratorInfo::prepare");

  Calibrator::Info* info = 0;

  if (calibrator_stokes)
  {
    CalibratorStokesInfo* stokes =
      new CalibratorStokesInfo (data->get<CalibratorStokes>());
    stokes->set_degree (calibrator_stokes_degree);
    info = stokes;
  }
  
  else if (reduced_chisq)
    info = new SolverInfo (new PolnCalibrator(data));

  else if (intrinsic_crosspol_ratio)
    info = new IXRInfo (new PolnCalibrator(data));

  else if (constant_gain)
    info = new ConstantGainInfo (new FluxCalibrator(data));

  else
    info = CalibratorParameter::get_Info (data, subint, outlier_threshold);

  get_frame()->get_label_above()->set_centre("$file\n"+info->get_title());

  unsigned nclass = info->get_nclass();

  vector<unsigned> classes;

  if (!panels.empty())
  {
    TextInterface::parse_indeces (classes, panels, nclass);
    nclass = classes.size();
  }
    
  // fraction of viewport reserved for total space between panels
  float yspace = 0;
  // fraction of viewport for space between each panel
  float ybetween = 0;

  if (nclass > 1)
  {
    yspace = between_panels;
    ybetween = yspace / float(nclass -1);
  }

  // fraction of viewport for each panel
  float yheight = (1.0 - yspace) / float(nclass);
  float ybottom = 0.0;

  float xmin = 0.0;
  float xmax = 0.0;

  for (unsigned iclass=0; iclass < nclass; iclass++)
  {
    unsigned jclass = iclass;

    if (classes.size())
      jclass = classes[iclass];

    Reference::To<CalibratorParameter> plot = new CalibratorParameter;

    string name = "panel" + tostring(jclass);
    manage (name, plot);

    plot->set_managed (true);
    plot->set_class (jclass);

    DEBUG("Pulsar::CalibratorInfo::prepare call CalibratorParameter::prepare");

    plot->prepare (info, data);

    float fmin = 0.0;
    float fmax = 0.0;

    get_scale()->get_minmax (fmin, fmax);

    if (xmin == 0.0)
      xmin = fmin;
    else if (fmin != 0.0)
      xmin = std::min (xmin, fmin);

    if (xmax == 0.0)
      xmax = fmax;
    else if (fmax != 0.0)
      xmax = std::max (xmax, fmax);

    plot->get_frame()->set_viewport (0,1, ybottom, ybottom+yheight);
    ybottom += yheight + ybetween;

    // horizontal y-axis enumeration
    plot->get_frame()->get_y_axis()->add_opt('V');

    // increase the displacement between the label and the axis
    plot->get_frame()->get_y_axis()->set_displacement( 3.5 );

    if (iclass > 0)
    {
      // remove x-axis label
      plot->get_frame()->get_x_axis()->set_label(" ");

      // remove x-axis enumeration
      plot->get_frame()->get_x_axis()->rem_opt('N');
    }
  }

  get_scale()->set_minmax (xmin, xmax);

}

void Pulsar::CalibratorInfo::set_calibrator_stokes_degree (bool x)
{
  calibrator_stokes = calibrator_stokes_degree = x;
}

bool Pulsar::CalibratorInfo::get_calibrator_stokes_degree () const
{
  return calibrator_stokes && calibrator_stokes_degree;
}

//! Get the text interface to the configuration attributes
TextInterface::Parser* Pulsar::CalibratorInfo::get_interface ()
{
  return new Interface (this);
}

Pulsar::CalibratorInfo::Interface::Interface (CalibratorInfo* instance)
{
  if (instance)
    set_instance (instance);

  add( &CalibratorInfo::get_outlier_threshold,
       &CalibratorInfo::set_outlier_threshold,
       "cutoff", "Outlier threshold (as in pac -K)" );

  add( &CalibratorInfo::get_between_panels,
       &CalibratorInfo::set_between_panels,
       "space", "Fraction of viewport for space between panels" );

  add( &CalibratorInfo::get_panels,
       &CalibratorInfo::set_panels,
       "panels", "Panels to be plotted" );

  add( &CalibratorInfo::get_calibrator_stokes,
       &CalibratorInfo::set_calibrator_stokes,
       "cal", "Plot the calibrator Stokes parameters" );

  add( &CalibratorInfo::get_calibrator_stokes_degree,
       &CalibratorInfo::set_calibrator_stokes_degree,
       "calp", "Plot calibrator Stokes parameters w/ degree of polarization" );

  add( &CalibratorInfo::get_reduced_chisq,
       &CalibratorInfo::set_reduced_chisq,
       "gof", "Plot the model goodness-of-fit" );

  add( &CalibratorInfo::get_intrinsic_crosspol_ratio,
       &CalibratorInfo::set_intrinsic_crosspol_ratio,
       "ixr", "Plot the intrinsic cross-polarization ratio");

  add( &CalibratorInfo::get_constant_gain,
       &CalibratorInfo::set_constant_gain,
       "gain", "Plot constant gain flux calibrator information");

  add( &CalibratorInfo::get_subint,
       &CalibratorInfo::set_subint,
       "subint", "Sub-integration from which to derive calibrator" );

}
