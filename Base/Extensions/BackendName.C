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
  name = extension.name;

  return *this;
}

//! Destructor
Pulsar::BackendName::~BackendName ()
{
}
