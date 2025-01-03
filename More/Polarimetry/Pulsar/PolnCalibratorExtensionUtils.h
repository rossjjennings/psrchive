//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2024 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/PolnCalibratorExtensionUtils.h

#ifndef __Pulsar_PolnCalibratorExtensionUtils_h
#define __Pulsar_PolnCalibratorExtensionUtils_h

#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/PolnCalibrator.h"

namespace Calibration
{
  //! Create a new transformation instance described by the extension for the specified channel
  MEAL::Complex2* new_transformation (const Pulsar::PolnCalibratorExtension*, unsigned ichan);

  //! Verify that Transformation parameter names match immutable parameter names of MEAL::Complex2
  void handle_parameter_names (MEAL::Complex2* to, const Pulsar::PolnCalibratorExtension::Transformation* from);

  //! Copy parameter names from MEAL::Complex2 object to PolnCalibratorExtension::Transformation object
  void handle_parameter_names (Pulsar::PolnCalibratorExtension::Transformation* to, const MEAL::Complex2* from);

  //! Copy all parameter values and uncertainties
  template<class T, class F>
  void copy (T* to, const F* from)
  {
    if (to->get_nparam() != from->get_nparam())
      throw Error (InvalidParam, "copy<To,From>",
      "to nparam=%d != from nparam=%d",
      to->get_nparam(), from->get_nparam());

    for (unsigned i=0; i<to->get_nparam(); i++)
      to->set_Estimate(i, from->get_Estimate(i));

    handle_parameter_names (to, from);
  }
}

#endif



