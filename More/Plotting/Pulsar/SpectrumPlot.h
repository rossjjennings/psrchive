//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/SpectrumPlot.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/15 13:11:23 $
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

  };

}

#endif
