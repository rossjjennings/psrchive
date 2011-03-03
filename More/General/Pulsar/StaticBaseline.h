//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Attic/StaticBaseline.h,v $
   $Revision: 1.1 $
   $Date: 2011/03/03 10:52:38 $
   $Author: straten $ */

#ifndef __Pulsar_StaticBaseline_h
#define __Pulsar_StaticBaseline_h

#include "Pulsar/BaselineEstimator.h"

namespace Pulsar {

  //! Finds a Profile baseline using an interative bounding method
  class StaticBaseline : public BaselineEstimator {

  public:

    //! Default constructor
    StaticBaseline ();

    //! Destructor
    ~StaticBaseline ();

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! Interface to the algorithm
    class Interface;

    //! Return a copy constructed instance of self
    StaticBaseline* clone () const;

  protected:

    //! Calculate the PhaseWeight
    void calculate (PhaseWeight* weight);

  };

}

#endif
