/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Telescope.h"
#include "Pulsar/Site.h"

//! Default constructor
Pulsar::Telescope::Telescope ()
  : Extension ("Telescope")
{
  name = "unknown";
  elevation = 0;
  mount = Horizon;
  primary = Parabolic;
  focus = PrimeFocus;
}

//! Copy constructor
Pulsar::Telescope::Telescope (const Telescope& telescope)
  : Extension ("Telescope")
{
  operator = (telescope);
}

//! Operator =
const Pulsar::Telescope&
Pulsar::Telescope::operator= (const Telescope& telescope)
{
  name = telescope.name;

  latitude = telescope.latitude;
  longitude = telescope.longitude;
  elevation = telescope.elevation;

  mount = telescope.mount;
  primary = telescope.primary;
  focus = telescope.focus;

  return *this;
}

//! Destructor
Pulsar::Telescope::~Telescope ()
{
}

//! Set the coordinates of the telescope based on known tempo codes
void Pulsar::Telescope::set_coordinates (const std::string& code)
{
  const Site* site = Site::location (code);

  double lat, lon, rad;
  site->get_sph (lat, lon, rad);

  latitude.setRadians( lat );
  longitude.setRadians( lon );
}
