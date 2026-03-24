/***************************************************************************
 *
 *   Copyright (C) 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Telescope.h"

#include "Horizon.h"    // like Parkes
#include "Meridian.h"   // like Hobart
#include "KrausType.h"  // like Nancay
#include "Fixed.h"      // like LOFAR

Mount* Pulsar::mount_factory (Pulsar::Telescope::Mount mount)
{
  switch (mount)
  {
  case Telescope::Horizon:
    return new Horizon;
  case Telescope::Meridian:
    return new Meridian;
  case Telescope::KrausType:
    return new KrausType;
  case Telescope::Fixed:
    return new Fixed;

  case Telescope::Equatorial:
  case Telescope::Mobile:
    return 0;  // no Directional-derived class for this type

  default:
    throw Error (InvalidState, "Pulsar::mount_factory", "unknown Telescope::Mount type");
  }
}

// return true if the mount naturally tracks celestial coordinates
bool Pulsar::naturally_celestial (Telescope::Mount mount)
{
  return mount == Telescope::Equatorial;
}

