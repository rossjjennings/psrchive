/***************************************************************************
 *
 *   Copyright (C) 2004-2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/IntegrationTI.h"
#include "Pulsar/Pointing.h"
#include "Pulsar/AuxColdPlasmaMeasures.h"

using namespace std;

Pulsar::Integration::Interface::Interface (Integration* _instance)
{
  add( &Integration::get_nchan, "nchan", "Number of frequency channels" );
  add( &Integration::get_npol,  "npol",  "Number of polarizations" );
  add( &Integration::get_nbin,  "nbin",  "Number of pulse phase bins" );

  add( &Integration::get_epoch,
       &Integration::set_epoch,
       "mjd", "Epoch (MJD)" );
 
  add( &Integration::get_duration,
       &Integration::set_duration,
       "duration", "Integration length (s)" );

  add( &Integration::get_folding_period,
       &Integration::set_folding_period, 
       "period", "Folding period (s)" );

  add( &Integration::get_gate_duty_cycle,
       &Integration::set_gate_duty_cycle,
       "gate", "Gate duty cycle (turns)" );

  // note that explicit casts are required for overloaded methods

  VGenerator<double> dgenerator;
  add_value(dgenerator( "freq", "Channel centre frequency (MHz)",
			( double (Integration::*) (unsigned) const )
			&Integration::get_centre_frequency,
			&Integration::set_centre_frequency,
			&Integration::get_nchan ));

  VGenerator<float> fgenerator;
  add_value(fgenerator( "wt", "Channel weight",
			&Integration::get_weight,
			&Integration::set_weight,
			&Integration::get_nchan ));

  add( &Integration::list_extensions,
       &Integration::edit_extensions,
       "ext", "list all extensions, or add/remove extension" );
    
  if (_instance)
    set_instance( _instance );
}

//! Set the instance
void Pulsar::Integration::Interface::set_instance (Pulsar::Integration* c)
{
  TextInterface::To<Integration>::set_instance (c);

  clean();

  for (unsigned iext=0; iext < instance->get_nextension(); iext++)
  {
    Integration::Extension* extension = instance->get_extension(iext);
    insert (extension->get_short_name(), extension->get_interface());
  }
}

TextInterface::Parser* Pulsar::Integration::Interface::clone()
{
  if( instance )
    return new Interface( instance );
  else
    return new Interface();
}

