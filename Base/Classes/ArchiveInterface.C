/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ArchiveInterface.h"
#include "Pulsar/ArchiveExtension.h"
#include "Pulsar/IntegrationTI.h"

#include "Pulsar/FITSAlias.h"

Pulsar::Archive::Interface::Interface( Archive *c )
{
  add( &Archive::get_filename, "file",    "Name of the file" );

  add( &Archive::get_nbin,     "nbin",    "Number of pulse phase bins" );
  add( &Archive::get_nchan,    "nchan",   "Number of frequency channels" );
  add( &Archive::get_npol,     "npol",    "Number of polarizations" );
  add( &Archive::get_nsubint,  "nsubint", "Number of sub-integrations" );

  add( &Archive::get_type,
       &Archive::set_type,
       "type", "Observation type (Pulsar, PolnCal, etc.)" );

  add( &Archive::get_telescope,
       &Archive::set_telescope,
       "site", "Telescope name" );

  add( &Archive::get_source,
       &Archive::set_source,
       "name", "Name of the source" );

  add( &Archive::get_coordinates,
       &Archive::set_coordinates,
       "coord", "Coordinates of the source" );

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
       "dmc", "Dispersion corrected (boolean)");

  add( &Archive::get_faraday_corrected,
       &Archive::set_faraday_corrected,
       "rmc", "Faraday Rotation corrected (boolean)" );

  add( &Archive::get_poln_calibrated,
       &Archive::set_poln_calibrated,
       "polc", "Polarization calibrated (boolean)" );

  add( &Archive::get_scale,
       &Archive::set_scale,
       "scale", "Units of profile amplitudes" );

  add( &Archive::get_state,
       &Archive::set_state,
       "state", "State of profile amplitudes" );
  
  add( &Archive::integration_length,
       "length", "The full duration of the observation (s)" );
  
  import( "int", IntegrationTI(),
          (Integration*(Archive::*)(unsigned)) &Archive::get_Integration,
          &Archive::get_nsubint );

  set_aliases( new FITSAlias );

  if (c)
    set_instance (c);
}

//! Set the instance
void Pulsar::Archive::Interface::set_instance (Pulsar::Archive* c) 
{
  TextInterface::To<Archive>::set_instance (c);

  nested_clean();

  for (unsigned iext=0; iext < instance->get_nextension(); iext++)
  {
    Archive::Extension* extension = instance->get_extension(iext);
    nested_import (extension->get_short_name(), extension->get_interface());
  }
}

TextInterface::Parser *Pulsar::Archive::Interface::clone()
{
  if( instance )
    return new Interface( instance );
  else
    return new Interface();
}

