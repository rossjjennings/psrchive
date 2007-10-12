//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Subtract.h,v $
   $Revision: 1.1 $
   $Date: 2007/10/12 02:45:48 $
   $Author: straten $ */

#ifndef __Pulsar_Subtract_h
#define __Pulsar_Subtract_h

#include "Pulsar/Combination.h"
#include "Pulsar/Profile.h"

namespace Pulsar {

  class Profile;

  //! Profile smoothing algorithms
  class Subtract : public Combination<Profile> {

  public:

    void transform (Profile*);

  }; 

}

#endif
