//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/SmoothSinc.h

#ifndef __Pulsar_SmoothSinc_h
#define __Pulsar_SmoothSinc_h

#include "Pulsar/Smooth.h"

namespace Pulsar {

  //! Smooths a Profile by convolution with a sinc function
  /*! 
    This method smooths the profile by low-pass filtering
    
  */
  class SmoothSinc : public Smooth {

  public:

    //! Return a copy constructed instance of self
    SmoothSinc* clone () const;

    //! Return a text-based interface to class properties
    TextInterface::Parser* get_interface ();

    //! Text-based interface to class properties
    class Interface;

    void transform (Profile*);

  };

}

#endif
