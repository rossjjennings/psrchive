//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PhaseVsFrequencyPlus.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/15 14:05:42 $
   $Author: straten $ */

#ifndef __Pulsar_PhaseVsFrequencyPlus_h
#define __Pulsar_PhaseVsFrequencyPlus_h

#include "Pulsar/MultiPlot.h"
#include "Pulsar/PhaseVsFrequency.h"
#include "Pulsar/ProfilePlot.h"
#include "Pulsar/SpectrumPlot.h"

namespace Pulsar {

  class PolnProfile;

  //! Plots phase vs. frequency, plus integrated profile and spectrum
  class PhaseVsFrequencyPlus : public MultiPlot {

  public:

    //! Default constructor
    PhaseVsFrequencyPlus ();

    //! Get the text interface to the configuration attributes
    TextInterface::Class* get_interface ();

    //! Ensure that frames are properly initialized
    void prepare (const Archive*);

    //! Set the sub-integration to plot (where applicable)
    void set_subint (unsigned _isubint) { isubint = _isubint; }
    unsigned get_subint () const { return isubint; }
    
    PhaseVsFrequency* get_freq () { return &freq; }
    ProfilePlot* get_flux () { return &flux; }
    SpectrumPlot* get_psd () { return &psd; }

  protected:

    PhaseVsFrequency freq;
    ProfilePlot flux;
    SpectrumPlot psd;

    unsigned isubint;

  };

}

#endif
