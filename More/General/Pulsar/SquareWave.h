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
#include "Estimate.h"

namespace Pulsar {

  class Profile;
  class Integration;
  class PhaseWeight;
  
  //! Calculates the signal-to-noise ratio using (on-off)/rms
  /*! This class also computes the levels used in calibration */
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

    void levels (const Integration* subint,
		 std::vector<std::vector<Estimate<double> > >& high,
		 std::vector<std::vector<Estimate<double> > >& low);

    //! Returns the phase bins that were not flagged as outliers
    PhaseWeight* get_mask (const Profile*, bool on, int start, int low);

    //! Set the rise time of the square wave in turns of phase
    void set_risetime (float turns) { risetime = turns; }

    //! Get the rise time of the square wave in turns of phase
    float get_risetime () const { return risetime; }

    //! Set the threshold for square wave detection
    void set_threshold (float turns) { threshold = turns; }

    //! Get the threshold for square wave detection
    float get_threshold () const { return threshold; }

    //! Set the number of square wave states
    void set_nstate (unsigned s) { nstate = s; }

    //! Get the number of square wave states
    unsigned get_nstate () const { return nstate; }

    //! Set the threshold used to reject outliers when computing levels
    void set_outlier_threshold (float f) { outlier_threshold = f; }

    //! Get the threshold used to reject outliers when computing levels
    float get_outlier_threshold () const { return outlier_threshold; }

    bool verbose;
    
  protected:

    float risetime;
    float threshold;
    unsigned use_nbin;
    unsigned nstate;
    float outlier_threshold;
  };

}

#endif
