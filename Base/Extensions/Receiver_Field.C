/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Receiver_Field.h"

Pulsar::Receiver::Field::Field ()
{
  symmetry_angle = false;
}

//! Get the orientation of the basis about the line of sight
Angle Pulsar::Receiver::Field::get_orientation () const
{
  Angle offset;
  if (symmetry_angle && basis == Signal::Linear)
    offset.setDegrees (-45);

  return orientation + offset;
}

//! Set the orientation of the basis about the line of sight
void Pulsar::Receiver::Field::set_orientation (const Angle& angle)
{
  orientation = angle;
  symmetry_angle = false;
}

//! Get the orientation of the equal in-phase electric field vector
Angle Pulsar::Receiver::Field::get_symmetry_angle () const
{
  Angle offset = 0.0;
  if (!symmetry_angle && basis == Signal::Linear)
    offset.setDegrees (45.0);
  
  return orientation + offset;
}

//! Set the orientation of the equal in-phase electric field vector
void Pulsar::Receiver::Field::set_symmetry_angle (const Angle& angle)
{
  orientation = angle;
  symmetry_angle = true;
}
