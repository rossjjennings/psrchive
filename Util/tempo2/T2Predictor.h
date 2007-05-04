//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo2/T2Predictor.h,v $
   $Revision: 1.1 $
   $Date: 2007/05/04 22:58:25 $
   $Author: straten $ */

#ifndef __Tempo2Predictor_h
#define __Tempo2Predictor_h

#include "Predictor.h"

namespace Tempo2 {

  //! Functions that predict pulse phase
  /*! This pure virtual base class defines the interface to pulse
    phase predictors */
  class Predictor : public Pulsar::Predictor {

  public:

    //! Return a new, copy constructed instance of self
    Pulsar::Predictor* clone () const;

    //! Return true if the supplied predictor is equal to self
    bool equals (const Pulsar::Predictor*) const;

    //! Set the observing frequency at which predictions will be made
    void set_observing_frequency (long double MHz);

    //! Get the observing frequency at which phase and epoch are related
    long double get_observing_frequency () const;

    //! Return the phase, given the epoch
    Phase phase (const MJD& t) const;

    //! Return the epoch, given the phase
    MJD iphase (const Phase& phase) const;

    //! Return the spin frequency, given the epoch
    long double frequency (const MJD& t) const;

    //! Return the phase correction for dispersion delay
    Phase dispersion (const MJD &t, long double MHz) const;

  };

}

#endif
