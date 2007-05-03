//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/Attic/Predictor.h,v $
   $Revision: 1.1 $
   $Date: 2007/05/03 21:05:34 $
   $Author: straten $ */

#ifndef __PulsarPredictor_h
#define __PulsarPredictor_h

#include "ReferenceAble.h"
#include "Phase.h"
#include "MJD.h"

namespace Pulsar {

  //! Functions that predict pulse phase
  /*! This pure virtual base class defines the interface to pulse
    phase predictors */
  class Predictor : public Reference::Able {

  public:

    //! Set the radio frequency at which predictions will be made
    virtual void set_radio_frequency (double MHz) = 0;

    //! Return the phase, given the epoch
    virtual Phase phase (const MJD& t) const = 0;

    //! Return the epoch, given the phase
    virtual MJD iphase (const Phase& phase) const = 0;

    //! Return the spin frequency, given the epoch
    virtual double frequency (const MJD& t) const = 0;

  };

}

#endif
