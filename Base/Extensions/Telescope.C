/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Telescope.h"
#include "coord.h"

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
void Pulsar::Telescope::set_coordinates (char tempo_isite)
{

  float lat, lon;

  int ret = telescope_coords (tempo_isite, &lat, &lon, &elevation);
  if (ret < 0){
    latitude.setDegrees( 0.0 );
    longitude.setDegrees( 0.0 );
    fprintf(stderr,"Hacked coords because failed to understand telescope site\n");

    //    throw Error (FailedCall, "Pulsar::Telescope::set_coordinates",
    //	 "tempo code=%c", tempo_isite);
  }

  latitude.setDegrees( lat );
  longitude.setDegrees( lon );

}
