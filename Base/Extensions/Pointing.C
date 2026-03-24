/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Pointing.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Telescope.h"
#include "Horizon.h"
#include "strutil.h"

// #define _DEBUG 1
#include "debug.h"

using namespace std;

Pulsar::Pointing::Pointing () : Extension ("Pointing")
{
}

Pulsar::Pointing::Pointing (const Pointing& that)
  : Extension ("Pointing")
{
  operator = (that);
}

const Pulsar::Pointing&
Pulsar::Pointing::operator= (const Pointing& that)
{
  local_sidereal_time = that.local_sidereal_time;
  right_ascension     = that.right_ascension;
  declination         = that.declination;
  galactic_longitude  = that.galactic_longitude;
  galactic_latitude   = that.galactic_latitude;
  feed_angle          = that.feed_angle;
  position_angle      = that.position_angle;
  parallactic_angle   = that.parallactic_angle;
  telescope_azimuth   = that.telescope_azimuth;
  telescope_zenith    = that.telescope_zenith;

  DEBUG("Pulsar::Pointing::operator= this.right_ascension=" << right_ascension << " that.right_ascension=" << that.right_ascension);

  return *this;
}

const Pulsar::Pointing&
Pulsar::Pointing::operator += (const Pointing& that)
{
  local_sidereal_time += that.local_sidereal_time;
  right_ascension     += that.right_ascension;
  declination         += that.declination;
  galactic_longitude  += that.galactic_longitude;
  galactic_latitude   += that.galactic_latitude;
  feed_angle          += that.feed_angle;
  position_angle      += that.position_angle;
  parallactic_angle   += that.parallactic_angle;
  telescope_azimuth   += that.telescope_azimuth;
  telescope_zenith    += that.telescope_zenith;

  return *this;
}

Pulsar::Pointing::~Pointing ()
{
}

static const double seconds_per_day = 24.0 * 60.0 * 60.0;
static const double radians_per_second = 2.0 * M_PI / seconds_per_day;

void Pulsar::Pointing::set_local_sidereal_time (double seconds)
{
  local_sidereal_time = Estimate<double> (seconds * radians_per_second, 1.0);
}

double Pulsar::Pointing::get_local_sidereal_time () const
{
  return local_sidereal_time.get_Estimate().val / radians_per_second;
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

Angle Pulsar::Pointing::get_hour_angle () const
{
  double lst_rad = get_local_sidereal_time() * radians_per_second;
  double ra_rad = get_right_ascension().getRadians();

  if (Integration::verbose)
    cerr << "Pulsar::Pointing::get_hour_angle lst=" << lst_rad << " R.A.=" << ra_rad << " (radians)" << endl;
  return lst_rad - ra_rad;
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


void Pulsar::Pointing::update (const Integration* subint)
{
  const Archive* archive = get_parent (subint);

  if (!archive)
    throw Error (InvalidState, "Pulsar::Pointing::update",
	         "Integration has no parent Archive");

  update(subint, archive);
}


/*! Based on the epoch of the Integration, uses slalib to re-calculate
  the following Pointing attributes: local_sidereal_time, parallactic_angle, 
  telescope_azimuth, and telescope_zenith. */
void Pulsar::Pointing::update (const Integration* subint, const Archive *archive)
{
  const Telescope* telescope = archive->get<Telescope>();

  if (!telescope)
    throw Error (InvalidState, "Pulsar::Pointing::update",
	         "parent Archive has no telescope Extension");

  sky_coord coord( get_right_ascension(), get_declination() );

  if (Integration::verbose)
    cerr << 
      "Pulsar::Pointing::update before:\n"
      "  lst=" << get_local_sidereal_time()/3600.0 << " hours\n"
      "   az=" << get_telescope_azimuth().getDegrees() << " deg\n"
      "  zen=" << get_telescope_zenith().getDegrees() << " deg\n"
      " para=" << get_parallactic_angle().getDegrees() << " deg\n"
      " posa=" << get_position_angle().getDegrees() << " deg\n"
      " r.a.=" << get_right_ascension().getDegrees() << " deg\n"
      " dec.=" << get_declination().getDegrees() << " deg\n"
      "Pulsar::Pointing::update parent:\n"
      " r.a.=" << archive->get_coordinates().ra().getDegrees() << " deg\n"
      " dec.=" << archive->get_coordinates().dec().getDegrees() << " deg"
	 << endl;

  if (coord.ra() == 0 && coord.dec() == 0)
  {
    coord = archive->get_coordinates();
    if (Integration::verbose)
      cerr << "Pulsar::Pointing::update using parent coordinates=" << coord << endl;

    set_right_ascension(coord.ra());
    set_declination(coord.dec());
  }

  Mount* mount = mount_factory (telescope->get_mount());
  if (!mount)
  {
    if (Integration::verbose)
      cerr << "Pulsar::Pointing::update no Mount for Telescope - update aborted" << endl;
    return;
  }

  if (Integration::verbose)
    cerr << "Pulsar::Pointing::update using observatory"
            " latitude=" << telescope->get_latitude() <<
            " longitude=" << telescope->get_longitude() << endl;

  mount->set_source_coordinates( coord );
  mount->set_observatory_latitude( telescope->get_latitude().getRadians() );
  mount->set_observatory_longitude( telescope->get_longitude().getRadians() );
  mount->set_epoch( subint->get_epoch() );

  if (Integration::verbose)
    cerr << "Pulsar::Pointing::update LST=" << mount->get_local_sidereal_time() << " rad" << endl;

  local_sidereal_time = Estimate<double>( mount->get_local_sidereal_time(), 1.0 );

  Directional* directional = dynamic_cast<Directional*> (mount);
  if (!directional)
  {
    if (Integration::verbose)
      cerr << "Pulsar::Pointing::update Mount for Telescope is not Directional - parallactic angle not updated" << endl;
  }
  else
  {
    set_parallactic_angle( directional->get_parallactic_angle() );
    set_position_angle( get_feed_angle() + get_parallactic_angle() );
  }

  Horizon* horizon = dynamic_cast<Horizon*> (mount);
  if (!horizon)
  {
    if (Integration::verbose)
      cerr << "Pulsar::Pointing::update Mount for Telescope is not Horizon - az and zen not updated" << endl;
  }
  else
  {
    set_telescope_azimuth( horizon->get_azimuth() );
    set_telescope_zenith( horizon->get_zenith() );
  }

  if (Integration::verbose)
    cerr << "Pulsar::Pointing::update after:\n"  
         "  lst=" << get_local_sidereal_time()/3600.0 << " hours\n"
         "   az=" << get_telescope_azimuth().getDegrees() << " deg\n"
         "  zen=" << get_telescope_zenith().getDegrees() << " deg\n"
         " para=" << get_parallactic_angle().getDegrees() << " deg\n"
         " posa=" << get_position_angle().getDegrees() << " deg" << endl;

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

  if (Integration::verbose)
    cerr << "Pulsar::Pointing::integrate that Pointing" << endl;

  operator += (*useful);
}


//! Return a text interfaces that can be used to access this instance
TextInterface::Parser* Pulsar::Pointing::get_interface()
{
  return new Interface( this );
}


unsigned Pulsar::Pointing::get_ninfo () const
{
  return info.size();
}

void Pulsar::Pointing::add_info (Info* _info)
{
  info.push_back( _info );
}

Pulsar::Pointing::Info* Pulsar::Pointing::get_info (unsigned i)
{
  return info.at(i);
}

const Pulsar::Pointing::Info* Pulsar::Pointing::get_info (unsigned i) const
{
  return info.at(i);
}

const Pulsar::Pointing::Info* 
Pulsar::Pointing::find_info (const std::string& name) const
{
  return const_cast<Pointing*>(this)->find_info (name);
}

Pulsar::Pointing::Info* Pulsar::Pointing::find_info (const std::string& name)
{
  for (auto _info: info)
    if (_info->get_name() == name)
      return _info;

  throw Error (InvalidParam, "Pulsar::Pointing::find_info",
               "no information named '%s'", name.c_str());
}

double Pulsar::Pointing::get_value (const std::string& name) const
{
  return find_info(name)->get_value();
}

void Pulsar::Pointing::set_value (const std::string& name, double val)
{
  find_info(name)->set_value(val);
}

TextInterface::Parser* 
Pulsar::Pointing::get_value_interface (const std::string& name)
{
  return find_info(name)->get_interface();
}

std::string Pulsar::Pointing::list_info () const
{
  string retval;

  unsigned next = get_ninfo();

  for (unsigned iext=0; iext<next; iext++)
  {
    if (iext>0)
      retval += ",";
    retval += get_info(iext)->get_name();
  }

  return retval;
}

void Pulsar::Pointing::edit_info (const std::string& name)
{
  string ext = name.substr(1);

  if (name[0] == '+')
  {
    Info* _info = new Info;
    _info->set_name (ext);
    add_info ( _info );
  }
  else if (name[0] == '-')
  {
    unsigned next = get_ninfo();

    for (unsigned i=0; i<next; i++)
      if (get_info(i)->get_name() == ext)
        delete get_info(i);
  }
  else
    throw Error (InvalidParam, "Pulsar::Integration::edit_infos",
                 "command starts with neither '+' nor '-'");
}

//! Return a text interfaces that can be used to access this instance
TextInterface::Parser* Pulsar::Pointing::Info::get_interface()
{
  return new Interface(this);
}

