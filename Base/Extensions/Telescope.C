#include "Pulsar/Telescope.h"

//! Default constructor
Pulsar::Telescope::Telescope ()
  : Extension ("Telescope")
{
  name = "unknown";
  mount = Horizon;
  primary = Parabolic;
  focus = PrimeFocus;
}

//! Copy constructor
Pulsar::Telescope::Telescope (const Telescope& extension)
  : Extension ("Telescope")
{
  operator = (extension);
}

//! Operator =
const Pulsar::Telescope&
Pulsar::Telescope::operator= (const Telescope& extension)
{
  name = extension.name;
  mount = extension.mount;
  primary = extension.primary;
  focus = extension.focus;

  return *this;
}

//! Destructor
Pulsar::Telescope::~Telescope ()
{
}
