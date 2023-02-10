//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/Pulsar/SpectrumPlot.h

#ifndef __Pulsar_SpectrumPlot_h
#define __Pulsar_SpectrumPlot_h

#include "Pulsar/PowerSpectra.h"
#include "Pulsar/HasPen.h"

namespace Pulsar {

  class ArchiveStatistic;

  //! Plots a single spectrum
  class SpectrumPlot : public PowerSpectra, public HasPen
  {

  public:

    // Default constructor
    SpectrumPlot ();

    // Destructor
    ~SpectrumPlot ();

    // Text interface to the SpectrumPlot class
    class Interface : public TextInterface::To<SpectrumPlot> {
    public:
      Interface (SpectrumPlot* = 0);
    };

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ()
    { return new Interface (this); }

    //! Load the spectra
    void get_spectra (const Archive* data);

    //! Set the phase bin to plot
    void set_bin (const Index& _ibin) { ibin = _ibin; }
    Index get_bin () const { return ibin; }

    //! Set the ProfileStats expression to evaluate
    void set_expression (const std::string& str) { expression = str; }
    std::string get_expression () const { return expression; }

    //! Set the statistic
    void set_statistic (ArchiveStatistic*);

    //! Get the statistic
    ArchiveStatistic* get_statistic () const;

    //! Disable baseline removal
    void preprocess (Archive*);

  protected:

    Index ibin;
    std::string expression;
    Reference::To<ArchiveStatistic> statistic;
  };

}

#endif
