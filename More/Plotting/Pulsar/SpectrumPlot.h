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

namespace Pulsar {

  //! Plots a single spectrum
  class SpectrumPlot : public PowerSpectra {

  public:

    // Default constructor
    SpectrumPlot ();

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

    //! Disable baseline removal
    void preprocess (Archive*);

  protected:

    Index ibin;
    std::string expression;

  };

}

#endif
