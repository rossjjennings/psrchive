//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/StandardSpectra.h,v $
   $Revision: 1.1 $
   $Date: 2008/03/03 04:15:01 $
   $Author: straten $ */

#ifndef __Calibration_StandardSpectra_H
#define __Calibration_StandardSpectra_H

#include "Pulsar/PolnSpectrumStats.h"
#include "MEAL/NormalizeStokes.h"

namespace Calibration {

  //! Computes the normalized complex Stokes parameters in each harmonic

  /*!
    This class is the Fourier domain equivalent of the StandardData class.
  */

  class StandardSpectra : public Reference::Able
  {
  public:
    
    //! Default constructor
    /*! If specified, baseline and last harmonic are defined by select */
    StandardSpectra (const Pulsar::PolnProfile* select = 0);

    //! Select the baseline and last harmonic from profile
    void select_profile (const Pulsar::PolnProfile*);

    //! Set the profile from which estimates will be derived
    void set_profile (const Pulsar::PolnProfile*);

    //! Normalize estimates by the average determinant
    void set_normalize (bool);

    //! Get the Stokes parameters of the specified phase bin
    Stokes< std::complex< Estimate<double> > > get_stokes (unsigned ibin);

 protected:

    Reference::To< Pulsar::PolnSpectrumStats > stats;
    Reference::To< MEAL::NormalizeStokes > normalize;

    Stokes< std::complex<double> > profile_variance;
    Estimate<double> total_determinant;

  };
  
}

#endif
