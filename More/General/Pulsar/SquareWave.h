//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/SquareWave.h

#ifndef __Pulsar_SquareWave_h
#define __Pulsar_SquareWave_h

#include "Pulsar/SNRatioEstimator.h"

namespace Pulsar {

  class Profile;

  //! Calculates the signal-to-noise ratio using (on-off)/rms
  class SquareWave : public SNRatioEstimator {

  public:

    //! Default constructor
    SquareWave ();
    
    //! Return the signal to noise ratio
    float get_snr (const Profile* profile);

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! The class that is returned by get_interface
    class Interface;

    //! Return a copy constructed instance of self
    SquareWave* clone () const;

    //! Search for multiple level transitions
    void get_transitions (const Profile* profile,
			  std::vector<unsigned>& up,
			  std::vector<unsigned>& down);

    //! Count the level transitions
    unsigned count_transitions (const Profile* profile);

    //! Set the rise time of the square wave in turns of phase
    void set_risetime (float turns) { risetime = turns; }

    //! Get the rise time of the square wave in turns of phase
    float get_risetime () const { return risetime; }

    //! Set the threshold for square wave detection
    void set_threshold (float turns) { threshold = turns; }

    //! Get the threshold for square wave detection
    float get_threshold () const { return threshold; }

  protected:

    float risetime;
    float threshold;
    unsigned use_nbin;

  };

}

#endif
