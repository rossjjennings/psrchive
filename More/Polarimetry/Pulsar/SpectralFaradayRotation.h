//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2026 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/SpectralFaradayRotation.h

#ifndef __Pulsar_SpectralFaradayRotation_H
#define __Pulsar_SpectralFaradayRotation_H

#include "MEAL/Variable.h"
#include "MEAL/Faraday.h"

namespace Pulsar
{
  //! Faraday rotation transformation with associated frequency axis
  class SpectralFaradayRotation : public MEAL::Variable<MEAL::Complex2>
  {
    MEAL::Faraday faraday;
    MEAL::Axis<double> frequency;

    public:
      SpectralFaradayRotation ()
      { frequency.signal.connect (&faraday, &MEAL::Faraday::set_frequency); }

      MEAL::Faraday* get_transformation () override { return &faraday; }
      MEAL::Argument* get_argument () override { return &frequency; }
      MEAL::Argument::Value* new_value (double freq) { return frequency.new_Value (freq); }
  };

}

#endif
