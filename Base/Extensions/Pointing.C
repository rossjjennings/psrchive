#include "Pulsar/Pointing.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Telescope.h"

#include "string_utils.h"
#include "coord.h"

Pulsar::Pointing::Pointing () : Extension ("Pointing")
{
}

Pulsar::Pointing::Pointing (const Pointing& extension) : Extension ("Pointing")
{
  operator = (extension);
}

const Pulsar::Pointing&
Pulsar::Pointing::operator= (const Pointing& extension)
{
  local_sidereal_time = local_sidereal_time;
  right_ascension = right_ascension;
  declination = declination;
  galactic_longitude = galactic_longitude;
  galactic_latitude = galactic_latitude;
  feed_angle = feed_angle;
  position_angle = position_angle;
  parallactic_angle = parallactic_angle;
  telescope_azimuth = telescope_azimuth;
  telescope_zenith = telescope_zenith;
  
  return *this;
}

const Pulsar::Pointing&
Pulsar::Pointing::operator += (const Pointing& extension)
{
  local_sidereal_time += local_sidereal_time;
  right_ascension += right_ascension;
  declination += declination;
  galactic_longitude += galactic_longitude;
  galactic_latitude += galactic_latitude;
  feed_angle += feed_angle;
  position_angle += position_angle;
  parallactic_angle += parallactic_angle;
  telescope_azimuth += telescope_azimuth;
  telescope_zenith += telescope_zenith;

  return *this;
}

Pulsar::Pointing::~Pointing ()
{
}


static const double seconds_per_day = 24.0 * 60.0 * 60.0;

void Pulsar::Pointing::set_local_sidereal_time (double seconds)
{
  local_sidereal_time = Estimate<double> (seconds*M_PI/seconds_per_day, 1.0);
}

double Pulsar::Pointing::get_local_sidereal_time () const
{
  return local_sidereal_time.get_Estimate().val * seconds_per_day / M_PI;
}

static void setmean (MeanRadian<double>& value, const Angle& angle)
{
  value = Estimate<double> (angle.getRadians(), 1.0);
}

static Angle getmean (const MeanRadian<double>& value)
{
  return value.get_Estimate().val;
}

void Pulsar::Pointing::set_right_ascension (const Angle& angle)
{
  setmean (right_ascension, angle);
}

Angle Pulsar::Pointing::get_right_ascension () const
{
  return getmean (right_ascension);
}


void Pulsar::Pointing::set_declination (const Angle& angle)
{
  setmean (declination, angle);
}

Angle Pulsar::Pointing::get_declination () const
{
  return getmean (declination);
}


void Pulsar::Pointing::set_galactic_longitude (const Angle& angle)
{
  setmean (galactic_longitude, angle);
}

Angle Pulsar::Pointing::get_galactic_longitude () const
{
  return getmean (galactic_longitude);
}


void Pulsar::Pointing::set_galactic_latitude (const Angle& angle)
{
  setmean (galactic_latitude, angle);
}

Angle Pulsar::Pointing::get_galactic_latitude () const
{
  return getmean (galactic_latitude);
}


void Pulsar::Pointing::set_feed_angle (const Angle& angle)
{
  setmean (feed_angle, angle);
}

Angle Pulsar::Pointing::get_feed_angle () const
{
  return getmean (feed_angle);
}


void Pulsar::Pointing::set_position_angle (const Angle& angle)
{
  setmean (position_angle, angle);
}

Angle Pulsar::Pointing::get_position_angle () const
{
  return getmean (position_angle);
}


void Pulsar::Pointing::set_parallactic_angle (const Angle& angle)
{
  setmean (parallactic_angle, angle);
}

Angle Pulsar::Pointing::get_parallactic_angle () const
{
  return getmean (parallactic_angle);
}


void Pulsar::Pointing::set_telescope_azimuth (const Angle& angle)
{
  setmean (telescope_azimuth, angle);
}

Angle Pulsar::Pointing::get_telescope_azimuth () const
{
  return getmean (telescope_azimuth);
}


void Pulsar::Pointing::set_telescope_zenith (const Angle& angle)
{
  setmean (telescope_zenith, angle);
}

Angle Pulsar::Pointing::get_telescope_zenith () const
{
  return getmean (telescope_zenith);
}




/*! Based on the epoch of the Integration, uses slalib to re-calculate
  the following Pointing attributes: lst, par_ang, tel_az, and
  tel_zen. */
void Pulsar::Pointing::update (const Integration* subint)
{
  const Archive* archive = get_parent (subint);

  if (!archive)
    throw (InvalidState, "Pulsar::Pointing::update",
	   "Integration has no parent Archive");

  const Telescope* telescope = archive->get<Telescope>();

  if (!telescope)
    throw (InvalidState, "Pulsar::Pointing::update",
	   "parent Archive has no telescope Extension");

  double latitude = telescope->get_latitude().getDegrees();
  double longitude = telescope->get_longitude().getDegrees();
  double lst = subint->get_epoch().LST(longitude);
    
  // correct the Pointing azimuth, zenith, and parallactic angles
  float azimuth=0, zenith=0, parallactic=0;
  if (az_zen_para (get_right_ascension().getRadians(),
		   get_declination().getRadians(), lst, latitude,
		   &azimuth, &zenith, &parallactic) < 0)
    throw (FailedCall, "Pulsar::Pointing::update", "az_zen_para");

  set_local_sidereal_time (lst * 3600.0);
  Angle angle;

  angle.setDegrees( parallactic );
  set_parallactic_angle( angle );

  angle.setDegrees( azimuth );
  set_telescope_azimuth( angle );

  angle.setDegrees( zenith );
  set_telescope_zenith( angle );


}


/*! Calculates the mean of the Pointing attributes. */
void Pulsar::Pointing::integrate (const Integration* subint)
{ 
  const Pointing* useful = subint->get<Pointing>();

  if (!useful) {
    if (Integration::verbose)
      cerr << "Pulsar::Pointing::integrate subint has no Pointing" << endl;
    return;
  }

  operator += (*useful);
}

