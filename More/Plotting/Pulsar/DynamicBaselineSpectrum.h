//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_DynamicBaselineSpectrum_h
#define __Pulsar_DynamicBaselineSpectrum_h

#include "Pulsar/DynamicSpectrum.h"
#include "Pulsar/PhaseWeight.h"

namespace Pulsar {

  //! Plots off-pulse mean or variance as a func of time and freq.
  class DynamicBaselineSpectrum : public DynamicSpectrum {

  public:

    //! Default constructor
    DynamicBaselineSpectrum ();

    //! Default constructor
    ~DynamicBaselineSpectrum ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the PhaseVsFrequency class
    class Interface : public TextInterface::To<DynamicBaselineSpectrum> {
    public:
      Interface (DynamicBaselineSpectrum* = 0);
    };

    void get_plot_array(const Archive *data, float *array);

    bool get_use_variance() const { return use_variance; }
    void set_use_variance(bool flag=true ) { use_variance = flag; }

    bool get_reuse_baseline() const { return reuse_baseline; }
    void set_reuse_baseline(bool flag=true ) { reuse_baseline = flag; }

  protected:

    bool use_variance;
    bool reuse_baseline;
    Reference::To<PhaseWeight> base;

  };

}

#endif
