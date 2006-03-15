//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/SpectrumPlot.h,v $
   $Revision: 1.2 $
   $Date: 2006/03/15 19:59:27 $
   $Author: straten $ */

#ifndef __Pulsar_SpectrumPlot_h
#define __Pulsar_SpectrumPlot_h

#include "Pulsar/PowerSpectra.h"

namespace Pulsar {

  //! Plots a single spectrum
  class SpectrumPlot : public PowerSpectra {

  public:

    //! Load the spectra
    void get_spectra (const Archive* data);

    //! Set the phase bin to plot
    void set_bin (const PlotIndex& _ibin) { ibin = _ibin; }
    PlotIndex get_bin () const { return ibin; }

  protected:

    PlotIndex ibin;

  };

}

#endif
