/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ConfigurableProjectionExtension.h"
#include "Pulsar/ConfigurableProjection.h"

using namespace std;

template<class T, class F>
void copy_estimates (T* to, const F* from)
{
  if (to->get_nparam() != from->get_nparam())
    throw Error (InvalidParam, "copy_estimates<To,From>",
                 "to nparam=%d != from nparam=%d",
                 to->get_nparam(), from->get_nparam());

  for (unsigned i=0; i<to->get_nparam(); i++)
    to->set_Estimate(i, from->get_Estimate(i));
}

void Pulsar::copy (MEAL::Complex2* to,
                   const ConfigurableProjectionExtension::Transformation* from)
{
  for (unsigned i=0; i<to->get_nparam(); i++)
  {
    string param_name = from->get_param_name(i);
    if (param_name != "" && param_name != to->get_param_name(i))
      throw Error (InvalidState, "copy_name",
		   "iparam=%d name=%s != model=%s",
		   i, param_name.c_str(), to->get_param_name(i).c_str());
  }

  copy_estimates (to, from);
}

void Pulsar::copy (ConfigurableProjectionExtension::Transformation* to,
                   const MEAL::Complex2* from)
{
  unsigned nparam = from->get_nparam();
  to->set_nparam( nparam );
  for (unsigned i=0; i<nparam; i++)
  {
    to->set_param_name(i, from->get_param_name(i));
    to->set_param_description(i, from->get_param_description(i));
  }

  copy_estimates (to, from);
}

//! Construct from a ConfigurableProjection instance
Pulsar::ConfigurableProjectionExtension::ConfigurableProjectionExtension
(const ConfigurableProjection* calibrator) try
  : HasChannels ("ConfigurableProjectionExtension") 
{
  if (!calibrator)
    throw Error (InvalidParam, "", "null ConfigurableProjection*");

  init ();

  if (Archive::verbose > 2)
    cerr << "Pulsar::ConfigurableProjectionExtension "
         "construct from ConfigurableProjection with config=\n" 
         << calibrator->get_configuration() << endl;

  set_configuration( calibrator->get_configuration() );

  has_covariance = false; // calibrator->has_covariance ();

  bool first = true;

  unsigned nchan = calibrator->get_nchan ();
  set_nchan (nchan);

  if (Archive::verbose > 2)
    cerr << "Pulsar::ConfigurableProjectionExtension nchan=" << nchan << endl;

  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    if ( ! calibrator->get_transformation_valid(ichan) )
    {
      if (Archive::verbose > 2)
        cerr << "Pulsar::ConfigurableProjectionExtension ichan=" << ichan 
             << " flagged invalid" << endl;
      
      set_valid (ichan, false);
      continue;
    }

    const ConfigurableProjection::Transformation* transformation = calibrator->get_transformation(ichan);
    const Calibration::VariableTransformation* xform = transformation->get_transformation();

    copy( get_transformation(ichan), xform );

    set_valid (ichan, true);

    if (first)
    {
      nparam = xform->get_nparam();

      if (Archive::verbose > 2)
      {
        const MEAL::Function* f = calibrator->get_transformation(ichan)->get_transformation();
        for (unsigned i=0; i<f->get_nparam(); i++)
          cerr << "Pulsar::ConfigurableProjectionExtension name[" << i << "]=" 
               << f->get_param_name(i) << endl;
      }
    }
    else if (get_nparam() != xform->get_nparam())
    {
      throw Error (InvalidParam, "Pulsar::ConfigurableProjectionExtension ctor",
                 "nparam=%d != Trnaformation[ichan=%d]:nparam=%d",
                 get_nparam(), ichan, xform->get_nparam());
    }

    first = false;

  }
}
catch (Error& error)
{
  throw error += "Pulsar::ConfigurableProjectionExtension (ConfigurableProjection*)";
}

