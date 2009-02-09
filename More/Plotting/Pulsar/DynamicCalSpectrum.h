//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_DynamicCalSpectrum_h
#define __Pulsar_DynamicCalSpectrum_h

#include "Pulsar/DynamicSpectrum.h"

namespace Pulsar {

  //! Plots off-pulse mean or variance as a func of time and freq.
  class DynamicCalSpectrum : public DynamicSpectrum {

  public:

    //! Default constructor
    DynamicCalSpectrum ();

    //! Default constructor
    ~DynamicCalSpectrum ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the PhaseVsFrequency class
    class Interface : public TextInterface::To<DynamicCalSpectrum> {
    public:
      Interface (DynamicCalSpectrum* = 0);
    };

    void get_plot_array(const Archive *data, float *array);

  };

}

#endif
