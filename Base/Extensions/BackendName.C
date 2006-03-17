/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/BackendName.h"

//! Default constructor
Pulsar::BackendName::BackendName ()
  : Backend ("BackendName")
{
  name = "unknown";
}

//! Copy constructor
Pulsar::BackendName::BackendName (const BackendName& ext)
  : Backend ("BackendName")
{
  operator = (ext);
}

//! Operator =
const Pulsar::BackendName&
Pulsar::BackendName::operator= (const BackendName& extension)
{
  Backend::operator = (extension);
  name = extension.name;

  return *this;
}

//! Destructor
Pulsar::BackendName::~BackendName ()
{
}
