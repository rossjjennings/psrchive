/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FluxCalManagerInfo.h"
#include "Pulsar/FluxCalManager.h"

#include <iostream>
#include <assert.h>

using namespace std;
using Calibration::FluxCalManager;

//! Constructor
Pulsar::FluxCalManagerInfo::FluxCalManagerInfo (const ReceptionCalibrator* cal)
{
  if (!cal->has_fluxcal())
    throw Error (InvalidParam, "FluxCalManagerInfo ctor",
		 "ReceptionCalibrator has no flux calibrator data");

  calibrator = cal;

  nclass = 0;

  if (!calibrator->multiple_flux_calibrators)
  {
    if (calibrator->model_fluxcal_on_minus_off)
    {
      nclass = 4;
      nclass_on = 2;
    }
    else
      nclass = nclass_on = 2;
  }
  else
  {
    unsigned nchan = get_nchan();
    unsigned nstate_on = 0;
    unsigned nstate_off = 0;
    
    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      const FluxCalManager* fluxcal = calibrator->get_fluxcal (ichan);
      nstate_on = std::max (fluxcal->get_nstate_on(), nstate_on);
      nstate_off = std::max (fluxcal->get_nstate_off(), nstate_off);
    }

    // two classes for each state: StokesI and StokesQUV
    nclass_on = nstate_on * 2;
    nclass = nclass_on + nstate_off * 2;
  }
}

//! Return the number of frequency channels
unsigned Pulsar::FluxCalManagerInfo::get_nchan () const
{
  return calibrator->get_nchan();
}

std::string Pulsar::FluxCalManagerInfo::get_title () const
{
  return "Flux Calibrator Stokes Parameters";
} 

//! Return the number of parameter classes
unsigned Pulsar::FluxCalManagerInfo::get_nclass () const
{
  return nclass;
}

//! Return the name of the specified class
std::string Pulsar::FluxCalManagerInfo::get_name (unsigned iclass) const
{
  string label = "\\fiS'\\b\\d\\fr";

  if (iclass < nclass_on)
  {
    if (calibrator->model_fluxcal_on_minus_off)
      label += "std";
    else
      label += "on";
  }
  else
    label += "off";

  label += ",";
  
  if (iclass % 2 == 0)
    // even -> Stokes I
    label += "0";
  else
    // odd -> Stokes QUV
    label += "p";
  
  return label;
}


//! Return the number of parameters in the specified class
unsigned Pulsar::FluxCalManagerInfo::get_nparam (unsigned iclass) const
{
  if (iclass % 2)
    return 3;  // odd -> Stokes QUV
  else
    return 1;  // even -> Stokes I
}

//! Return the estimate of the specified parameter
Estimate<float> 
Pulsar::FluxCalManagerInfo::get_param (unsigned ichan, unsigned iclass,
				       unsigned iparam) const
{
  if (!calibrator->get_transformation_valid (ichan))
    return 0.0;

  unsigned ipol = 0;

  if (iclass % 2)
    ipol = iparam + 1;  // off -> Stokes QUV

  const FluxCalManager* fluxcal = calibrator->get_fluxcal (ichan);

  if (!fluxcal)
    return 0.0;
  
  unsigned istate = iclass / 2;

#if _DEBUG
  cerr << "FluxCalManagerInfo::get_param ichan=" << ichan
       << " iclass=" << iclass << " iparam=" << iparam
       << " istate=" << istate << " ipol=" << ipol << endl;
#endif

  if (iclass < nclass_on)
    return fluxcal->get_source_on(istate)->source->get_stokes()[ipol];
  else
    return fluxcal->get_source_off(istate)->source->get_stokes()[ipol];
}


int Pulsar::FluxCalManagerInfo::get_colour_index (unsigned iclass, unsigned iparam) const
{
  if (iclass % 2)
    return iparam + 2;  // QUV = RGB
  else
    return 1;  // I = black
}
