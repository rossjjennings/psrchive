#include "Pulsar/PointingTUI.h"

Pulsar::PointingTUI::PointingTUI ()
{
  init ();
}

void Pulsar::PointingTUI::init ()
{
  {
    Generator<double> gen;
    add( gen.described ("lst", "Local sidereal time (seconds)",
                        &Pointing::get_local_sidereal_time,
                        &Pointing::set_local_sidereal_time) );
  }

  {
    Generator<Angle> gen;

    add( gen.described ("gb", "Galactic longitude (deg)",
                        &Pointing::get_galactic_longitude,
                        &Pointing::set_galactic_longitude) );

    add( gen.described ("gl", "Galactic latitude (deg)",
                        &Pointing::get_galactic_latitude,
                        &Pointing::set_galactic_latitude) );

    add( gen.described ("fa", "Feed angle (deg)",
                        &Pointing::get_feed_angle,
                        &Pointing::set_feed_angle) );

    add( gen.described ("pa", "Position angle of the feed (deg)",
                        &Pointing::get_position_angle,
                        &Pointing::set_position_angle) );

    add( gen.described ("va", "Vertical (parallactic) angle (deg)",
                        &Pointing::get_parallactic_angle,
                        &Pointing::set_parallactic_angle) );

    add( gen.described ("az", "Telescope azimuth",
			&Pointing::get_telescope_azimuth,
			&Pointing::set_telescope_azimuth) );

    add( gen.described ("zen", "Telescope zenith",
                        &Pointing::get_telescope_zenith,
                        &Pointing::set_telescope_zenith) );

  }
}

