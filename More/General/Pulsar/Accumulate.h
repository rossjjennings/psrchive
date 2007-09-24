//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Accumulate.h,v $
   $Revision: 1.1 $
   $Date: 2007/09/24 08:52:25 $
   $Author: straten $ */

#ifndef __Pulsar_Accumulate_h
#define __Pulsar_Accumulate_h

#include "Pulsar/Transformation.h"

namespace Pulsar {

  class Profile;

  //! Profile differentiation algorithm
  class Accumulate : public Transformation<Profile> {

  public:

    //! Default constructor
    Accumulate ();

    //! Destructor
    ~Accumulate ();

    //! Set the offset
    void set_offset (unsigned);

    //! Get the offset
    unsigned get_offset () const;

    //! Form the cummulative profile
    void transform (Profile* profile);

  protected:

    //! The offset
    unsigned offset;

  };

}

#endif
