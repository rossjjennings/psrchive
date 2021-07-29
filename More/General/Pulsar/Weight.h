//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/Weight.h

#ifndef _Pulsar_Weight_H
#define _Pulsar_Weight_H

#include "Pulsar/Algorithm.h"
#include "Pulsar/ScrunchFactor.h"
#include "TextInterface.h"

namespace Pulsar {
  
  class Archive;
  class Integration;

  //! Algorithms that set the weights of all Profiles in an Archive
  class Weight : public Algorithm {
    
  public:

    //! Default constructor
    Weight ();
    
    //! Set the weights of all Profiles in the Archive
    virtual void operator () (Archive*);

    //! Return a text interface that can be used to configure this instance
    virtual TextInterface::Parser* get_interface ();

    //! Text-based interface to Smooth class properties
    class Interface;

    //! Set the frequency scrunch factor
    void set_fscrunch (const ScrunchFactor& f) { fscrunch_factor = f; }

    //! Get the frequency scrunch factor
    const ScrunchFactor get_fscrunch () const { return fscrunch_factor; }

  protected:

    //! Set the weight of each frequency channel in the sub-integration
    virtual void weight (Integration*) = 0;

    //! Copy the weights from 'from' to 'to'
    void copy_weights (const Integration* from, Integration* to);

    //! Compute weights from fscrunched clone of data, then apply to original
    ScrunchFactor fscrunch_factor;

  };
  
}

#endif

