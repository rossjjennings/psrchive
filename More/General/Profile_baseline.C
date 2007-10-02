/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Profile.h"
#include "Pulsar/BaselineInterpreter.h"

using namespace Pulsar;

/*! 
  This use of the Functor template implements the Strategy design
  pattern (ghjv94 p.315) for calculating the profile baseline mask.
*/
Functor< PhaseWeight* (const Profile*) > Profile::baseline_strategy;


/*!  
  The BaselineInterpreter class sets the baseline_strategy
  attribute according to commands specified either in the
  configuration file or via the psrsh interpreter.  It enables
  convenient experimentation with the baseline estimation algorithm.
*/
static Pulsar::Option<CommandParser>
baseline ( new Pulsar::BaselineInterpreter, "Profile::baseline", "minimum" );


//! Return a PhaseWeight mask with the baseline phase bins enabled
Pulsar::PhaseWeight* Pulsar::Profile::baseline () const try
{
  return baseline_strategy (this);
}
catch (Error& error) {
  throw error += "Pulsar::Profile::baseline";
}
