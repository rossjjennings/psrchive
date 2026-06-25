//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/StandardFluctSpect.h

#ifndef __Calibration_StandardFluctSpect_H
#define __Calibration_StandardFluctSpect_H

#include "Pulsar/PolnFluctSpectStats.h"
#include "MEAL/NormalizeStokes.h"

namespace Calibration {

  //! Computes the normalized complex-valued Stokes parameters in each harmonic
  /*!
    The harmonics are those of the fluctuation power spectrum.
    This class is the Fourier domain equivalent of the StandardData class.
  */
  class StandardFluctSpect : public Reference::Able
  {
  public:
    
    //! Default constructor
    /*! If specified, baseline and last harmonic are defined by select */
    StandardFluctSpect (const Pulsar::PolnProfile* select = 0);

    //! Select the baseline and last harmonic from profile
    void select_profile (const Pulsar::PolnProfile*);

    //! Set the profile from which estimates will be derived
    void set_profile (const Pulsar::PolnProfile*);

    //! Get the fourier transform of the last set profile
    const Pulsar::PolnProfile* get_fourier () const;

    //! Return the index of the last harmonic
    unsigned get_last_harmonic () const;

    //! Normalize estimates by the average squared_invariant
    void set_normalize (bool);

    //! Get the Stokes parameters of the specified phase bin
    Stokes< std::complex< Estimate<double> > > get_stokes (unsigned ibin);

    //! Get the statistical interface to the data
    Pulsar::PolnFluctSpectStats* get_stats ();

    //! Get the total squared invariant used to normalize the Stokes parameters
    Estimate<double> get_total_squared_invariant() { return total_squared_invariant; }
    
 protected:

    Reference::To< Pulsar::PolnFluctSpectStats > stats;
    Reference::To< MEAL::NormalizeStokes > normalize;

    Estimate<double> total_squared_invariant;
    unsigned last_harmonic;

  };
  
}

#endif
