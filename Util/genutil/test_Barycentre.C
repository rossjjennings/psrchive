/***************************************************************************
 *
 *   Copyright (C) 2024 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*
	This program performs a simple conceptual test of the Barycentre::get_Doppler
	method by placing a pulsar at the vernal equinox (R.A.=0, Dec.=0) and verifying
	that the Doppler correction, D = 1 - v/c, is roughly consistent with 
	(within 10% of) the expected velocity of the Earth, assuming a circular orbit.

	Four epochs are tested, corresponding to the 2 equinoxes and 2 solstices.
*/

#include "Barycentre.h"
#include <string.h>

using namespace std;

int main ()
{
  const char* date_format = "%Y-%m-%d %H:%M:%S";

  struct tm vernal_equinox;
  memset(&vernal_equinox, 0, sizeof(struct tm));
  strptime("2024-03-20 00:03:06", date_format, &vernal_equinox);

  MJD mjd (vernal_equinox);

  sky_coord coordinates;
  coordinates.ra().setRadians( 0.0 );
  coordinates.dec().setRadians( 0.0 );

  Barycentre bary;
  bary.set_coordinates (coordinates);
  bary.set_observatory_xyz (0, 0, 0);

  double seconds_per_sidereal_year = 31558149.5;
  double offset_seconds = seconds_per_sidereal_year / 4;

  double expected_velocity[4];

  double speed_of_light = 299792458;  // m/s
  // Earth's orbital speed as a fraction of the speed of light
  double Earth_orbital_speed = 29784.8 / speed_of_light;

  /* On March 20, the Sun is at the vernal equinox, eclipsing the pulsar, and the Earth's velocity is perpendicular to the line of sight. */
  expected_velocity[0] = 0.0;

  /* Three months later, on June 20, the Earth is moving with maximum speed toward the pulsar. */
  expected_velocity[1] = Earth_orbital_speed;

  /* Six months later, on Sept 20, the Earth's velocity is once again perpendicular to the line of sight. */
  expected_velocity[2] = 0.0;

  /* Nine months later, on Dec 20, the Earth is moving with maximum speed away from the pulsar. */
  expected_velocity[3] = -Earth_orbital_speed;

  /* This tolerance is quite genereous because this simple test does not account for the ellipticity of Earth's orbit; therefore,
     the speeds are not exact (e.g. v_aphelion [3 Jan] = 29.29 km/s and v_perihelion [3 Jul] = 30.29 km/s) and Earth's velocity 
	 may not be exactly perpendicular to the line of apsides at the equinoxes. */

  double tolerance = 1e-5;

  MJD epoch = vernal_equinox;

  for (unsigned i=0; i<4; i++)
  {
	cerr << "epoch = " << epoch.datestr(date_format) << endl;
    bary.set_epoch (epoch);
    double D = bary.get_Doppler();

	// Barycentre::get_Doppler returns D=1-v/c
	double v_on_c = 1-D;
    cerr << "v/c = 1-D = " << v_on_c << endl;

	double diff = v_on_c - expected_velocity[i];
	if (fabs(diff) > tolerance)
	{
		cerr << "velocity calcuated by Barycentre::get_Doppler differs from expected value=" << expected_velocity[i] << endl;
		return -1;
	}
	epoch += offset_seconds;
  }
  return 0;
}
