//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/IQRBaseline.h

#ifndef __Pulsar_IQRBaseline_h
#define __Pulsar_IQRBaseline_h

#include "Pulsar/IterativeBaseline.h"

namespace Pulsar {

  //! Finds a baseline that falls within the the inter-quartile range
  class IQRBaseline : public IterativeBaseline
  {

  public:

    //! Default constructor
    IQRBaseline ();

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! Interface to the algorithm
    class Interface;

    //! Return a copy constructed instance of self
    IQRBaseline* clone () const;

  protected:

    void get_bounds (PhaseWeight* weight, float& lower, float& upper);

  };

}

#endif
