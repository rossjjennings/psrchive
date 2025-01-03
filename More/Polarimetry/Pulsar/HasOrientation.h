//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2024 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/HasOrientation.h

#ifndef __Pulsar_HasOrientation_h
#define __Pulsar_HasOrientation_h

#include "Estimate.h"

namespace Calibration
{
  //! Interface to transformations that have a feed rotation that can be offset
  class HasOrientation
  {   
  public:

    //! Get the rotation of the feed about the line of sight, in radians
    virtual Estimate<double> get_orientation () const = 0;

    //! Offset the rotation of the feed about the line of sight by delta, in radians
    virtual void offset_orientation (double delta_rad) = 0;

  };
}

#endif



