/***************************************************************************
 *
 *   Copyright (C) 2004-2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ArchiveInterface.h"
#include "Pulsar/ArchiveExtension.h"
#include "Pulsar/IntegrationInterface.h"
#include "Pulsar/FITSAlias.h"

#include "TextInterfaceEmbed.h"
#include "TextInterfaceTransform.h"

using namespace std;

static unsigned instance_count = 0;
unsigned Pulsar::Archive::Interface::get_instance_count () { return instance_count; }

double get_declination (const sky_coord& coord)
{
  return coord.dec().getRadians();
}

double get_right_ascension (const sky_coord& coord)
{
  return coord.ra().getRadians();
}

Pulsar::Archive::Interface::Interface( Archive *c )
{
  instance_count ++;

  add( &Archive::get_filename, "file",    "Name of the file" );

  add( &Archive::get_nbin,     "nbin",    "Number of pulse phase bins" );
  add( &Archive::get_nchan,    "nchan",   "Number of frequency channels" );
  add( &Archive::get_npol,     "npol",    "Number of polarizations" );
  add( &Archive::get_nsubint,  "nsubint", "Number of sub-integrations" );

  add( &Archive::get_type,
       &Archive::set_type,
       "type", "Observation type" );

  add( &Archive::get_telescope,
       &Archive::set_telescope,
       "site", "Telescope name" );

  add( &Archive::get_source,
       &Archive::set_source,
       "name", "Source name" );

  add( &Archive::get_coordinates,
       &Archive::set_coordinates,
       "coord", "Source coordinates" );

  TextInterface::XAllocator<Archive> xform;
  add_value( xform ("dec", "Declination (rad)",
                    &Archive::get_coordinates, get_declination) );

  add_value( xform ("ra", "Right Ascension (rad)",
                    &Archive::get_coordinates, get_right_ascension) );

  add( &Archive::get_centre_frequency,
       &Archive::set_centre_frequency,
       "freq", "Centre frequency (MHz)" );

  add( &Archive::get_bandwidth,
       &Archive::set_bandwidth,
       "bw", "Bandwidth (MHz)" );

  add( &Archive::get_dispersion_measure,
       &Archive::set_dispersion_measure,
       "dm", "Dispersion measure (pc/cm^3)" );

  add( &Archive::get_rotation_measure,
       &Archive::set_rotation_measure,
       "rm", "Rotation measure (rad/m^2)" );

  add( &Archive::get_dedispersed,
       &Archive::set_dedispersed,
       "dmc", "Dispersion corrected");

  add( &Archive::get_faraday_corrected,
       &Archive::set_faraday_corrected,
       "rmc", "Faraday Rotation corrected" );

  add( &Archive::get_poln_calibrated,
       &Archive::set_poln_calibrated,
       "polc", "Polarization calibrated" );

  add( &Archive::get_scale,
       &Archive::set_scale,
       "scale", "Data units" );

  add( &Archive::get_state,
       &Archive::set_state,
       "state", "Data state" );
  
  add( &Archive::integration_length,
       "length", "Observation duration (s)" );
  
  set_aliases( new FITSAlias );

  if (c)
    set_instance (c);
}

Pulsar::Archive::Interface::~Interface()
{
  instance_count --;
}

//! Set the instance
void Pulsar::Archive::Interface::set_instance (Pulsar::Archive* c) 
{
  TextInterface::To<Archive>::set_instance (c);

  clean();

  // pointer to Archive method that returns mutable Integration*
  // used to disambiguate const and non-const get_Integration
  typedef Integration*(Archive::*mutable_Integration)(unsigned);

  if (instance->get_nsubint() > 0)
  {
    TextInterface::Embed<Archive> embed;
    add_value ( embed( "int", 
			 (mutable_Integration) &Archive::get_Integration,
			 &Archive::get_nsubint ) );
  }

  for (unsigned iext=0; iext < instance->get_nextension(); iext++)
  {
    Archive::Extension* extension = instance->get_extension(iext);
    insert (extension->get_short_name(), extension->get_interface());
  }
}

TextInterface::Parser *Pulsar::Archive::Interface::clone()
{
  if( instance )
    return new Interface( instance );
  else
    return new Interface();
}

