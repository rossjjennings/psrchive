//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/genutil/MountProjection.h

#ifndef __MountProjection_H
#define __MountProjection_H

#include "Mount.h"
#include "Jones.h"

//! A Mount that can compute its own projection transformation
class MountProjection : public Mount
{  
public:
  
  //! Get the response
  virtual Jones<double> get_response () const = 0;

};

#endif

