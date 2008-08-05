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
static Pulsar::Option<CommandParser> cfg
(
 new Pulsar::BaselineInterpreter (Profile::baseline_strategy),
 "Profile::baseline", "minimum",

 "Baseline estimation algorithm",

 "The name of the algorithm used to estimate the off-pulse baseline.\n"
 "Possible values: minimum, normal"
);


//! Return a PhaseWeight mask with the baseline phase bins enabled
Pulsar::PhaseWeight* Pulsar::Profile::baseline () const try
{
  return baseline_strategy (this);
}
catch (Error& error) {
  throw error += "Pulsar::Profile::baseline";
}
