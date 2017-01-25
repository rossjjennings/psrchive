//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/SmoothMean.h

#ifndef __Pulsar_SmoothMean_h
#define __Pulsar_SmoothMean_h

#include "Pulsar/Smooth.h"

namespace Pulsar {

  //! Smooths a Profile using the mean over a boxcar
  /*! 
    This method smooths the profile by setting each amplitude equal to
    the mean calculated over the region centred at that point and with
    width specified by bins or turns.
  */
  class SmoothMean : public Smooth {

  public:

    //! Return a copy constructed instance of self
    SmoothMean* clone () const;

    //! Return a text-based interface to class properties
    TextInterface::Parser* get_interface ();

    //! Text-based interface to class properties
    class Interface;

    void transform (Profile*);

  };

}

#endif
