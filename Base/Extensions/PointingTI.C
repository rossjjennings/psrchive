/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Pointing.h"
#include "MethodFunction.h"

using Pulsar::Pointing;

Pointing::Interface::Interface ( Pointing *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  add( &Pointing::get_local_sidereal_time,
       &Pointing::set_local_sidereal_time,
       "lst", "Local sidereal time (seconds)" );

  add( &Pointing::get_galactic_longitude,
       &Pointing::set_galactic_longitude,
       "gb", "Galactic longitude (deg)" );

  add( &Pointing::get_galactic_latitude,
       &Pointing::set_galactic_latitude,
       "gl", "Galactic latitude (deg)" );

  add( &Pointing::get_feed_angle,
       &Pointing::set_feed_angle,
       "fa", "Feed angle (deg)" );

  add( &Pointing::get_position_angle,
       &Pointing::set_position_angle,
       "pa", "Position angle of the feed (deg)" );

  add( &Pointing::get_parallactic_angle,
       &Pointing::set_parallactic_angle,
       "va", "Vertical (parallactic) angle (deg)" );

  add( &Pointing::get_telescope_azimuth,
       &Pointing::set_telescope_azimuth,
       "az", "Telescope azimuth" );

  add( &Pointing::get_telescope_zenith,
       &Pointing::set_telescope_zenith,
       "zen", "Telescope zenith" );

  add( &Pointing::list_info,
       &Pointing::edit_info,
       "info", "list additional info, or add/remove info" );

  if (!instance)
    return;

  for (unsigned i=0; i < instance->get_ninfo(); i++)
  {
    Info* info = instance->get_info (i);
    add ( method_function (&Pointing::get_value, info->get_name()),
          method_function (&Pointing::set_value, info->get_name()),
          method_function (&Pointing::get_value_interface, info->get_name()),
          info->get_name().c_str(), info->get_description().c_str() );
  }

}

Pointing::Info::Interface::Interface ( Pointing::Info *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  add( &Pointing::Info::get_name,
       &Pointing::Info::set_name,
       "name", "name of the information" );

  add( &Pointing::Info::get_unit,
       &Pointing::Info::set_unit,
       "unit", "physical unit of the information" );

  add( &Pointing::Info::get_description,
       &Pointing::Info::set_description,
       "desc", "description of the information" );

  add( &Pointing::Info::get_value,
       &Pointing::Info::set_value,
       "value", "numerical value of the information" );
}

