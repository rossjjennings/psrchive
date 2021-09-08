//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2021 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_DynamicStatSpectrumPlot_h
#define __Pulsar_DynamicStatSpectrumPlot_h

#include "Pulsar/DynamicSpectrumPlot.h"
#include "Pulsar/ArchiveStatistic.h"

namespace Pulsar {

  //! Plots off-pulse mean or variance as a func of time and freq.
  class DynamicStatSpectrumPlot : public DynamicSpectrumPlot {

  public:

    //! Default constructor
    DynamicStatSpectrumPlot ();

    //! Default constructor
    ~DynamicStatSpectrumPlot ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the PhaseVsFrequency class
    class Interface : public TextInterface::To<DynamicStatSpectrumPlot> {
    public:
      Interface (DynamicStatSpectrumPlot* = 0);
    };

    void get_plot_array(const Archive *data, float *array);

    //! Disable preprocessing
    virtual void preprocess (Archive*) {} 

    //! Set the statistic
    void set_statistic (ArchiveStatistic*);

    //! Get the statistic
    ArchiveStatistic* get_statistic () const;

  protected:

    Reference::To<ArchiveStatistic> statistic;

  };

}

#endif
