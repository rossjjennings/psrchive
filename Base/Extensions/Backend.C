#include "Pulsar/Backend.h"

//! Default constructor
Pulsar::Backend::Backend ()
  : Extension ("Backend")
{
  name = "unknown";
}

//! Copy constructor
Pulsar::Backend::Backend (const Backend& extension)
  : Extension ("Backend")
{
  operator = (extension);
}

//! Operator =
const Pulsar::Backend&
Pulsar::Backend::operator= (const Backend& extension)
{
  name = extension.name

  return *this;
}

//! Destructor
Pulsar::Backend::~Backend ()
{
}
