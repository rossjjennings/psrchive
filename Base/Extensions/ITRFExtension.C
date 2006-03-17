/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ITRFExtension.h"

//! Default constructor
Pulsar::ITRFExtension::ITRFExtension ()
  : Extension ("ITRFExtension")
{
  ant_x = 0.0;
  ant_y = 0.0;
  ant_z = 0.0;
}

//! Copy constructor
Pulsar::ITRFExtension::ITRFExtension (const ITRFExtension& extension)
  : Extension ("ITRFExtension")
{
  ant_x = extension.ant_x;
  ant_y = extension.ant_y;
  ant_z = extension.ant_z;
}

//! Operator =
const Pulsar::ITRFExtension&
Pulsar::ITRFExtension::operator= (const ITRFExtension& extension)
{
  ant_x = extension.ant_x;
  ant_y = extension.ant_y;
  ant_z = extension.ant_z;

  return *this;
}

//! Destructor
Pulsar::ITRFExtension::~ITRFExtension ()
{
}
