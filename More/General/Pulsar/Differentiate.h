//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/Differentiate.h

#ifndef __Pulsar_Differentiate_h
#define __Pulsar_Differentiate_h

#include "Pulsar/Transformation.h"

namespace Pulsar {

  class Profile;

  //! Profile differentiation algorithm
  class Differentiate : public Transformation<Profile> {

  public:

    //! Default constructor
    Differentiate (unsigned span = 1);

    //! Destructor
    ~Differentiate ();

    //! Set the phase bin offset added and subtracted from each bin
    void set_span (unsigned);

    //! Get the phase bin offset added and subtracted from each bin
    unsigned get_span () const;

    //! Differentiate the given Profile
    void transform (Profile* profile);

  protected:

    //! The number of phase bins in the window used to smooth
    unsigned span;

  };

}

#endif
