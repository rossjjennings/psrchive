//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/FortranSNR.h

#ifndef __Pulsar_FortranSNR_h
#define __Pulsar_FortranSNR_h

#include "Pulsar/SNRatioEstimator.h"
#include "PhaseRange.h"

#include <vector>

namespace Pulsar {

  //! Calculates the signal-to-noise ratio by fitting against a standard
  class FortranSNR : public SNRatioEstimator
  {

  public:

    FortranSNR(); 
    
    //! Return the signal to noise ratio based on the shift
    float get_snr (const Profile* profile);

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! The class that is returned by get_interface
    class Interface;

    //! Return a copy constructed instance of self
    FortranSNR* clone () const;

    int get_bestwidth(){ return bestwidth; }

    void set_rms( float new_rms ) { rms = new_rms; rms_set = true; }
    void set_minwidthbins ( int bins ) { minwidthbins = bins; }
    void set_maxwidthbins ( int bins ) { maxwidthbins = bins; }
    void set_bestwidth(int bw) { bestwidth=bw; };

    Phase::Value get_width () const;

  private:
    float rms;
    int minwidthbins;
    int maxwidthbins;
    int bestwidth;
    bool rms_set;

    std::vector<float> work;
  };

}

#endif

