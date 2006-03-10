#include "Pulsar/GraphFactory.h"
#include "Pulsar/SinglePlotter.h"
#include "Pulsar/StokesPlotter.h"
#include "Pulsar/PhaseVsFrequency.h"
#include "Pulsar/PhaseVsTime.h"
#include "Pulsar/PosAngPlotter.h"
#include "Pulsar/EllAngPlotter.h"
#include "Pulsar/StokesCylindrical.h"
#include "Pulsar/StokesSpherical.h"
#include "Error.h"

// Add plots here
Pulsar::GraphFactory::GraphFactory ()
{
  add ( new GraphFactory::Advocate<SinglePlotter>
	("flux", "Single plot of flux") );

  add ( new GraphFactory::Advocate<StokesPlotter>
	("stokes", "Stokes parameters") );

  add ( new GraphFactory::Advocate<PhaseVsFrequency>
	("freq", "Phase vs. frequency image of flux") );

  add ( new GraphFactory::Advocate<PhaseVsTime>
	("time", "Phase vs. time image of flux") );

  add ( new GraphFactory::Advocate<PosAngPlotter>
	("pa", "Position angle") );

  add ( new GraphFactory::Advocate<EllAngPlotter>
	("ell", "Ellipticity angle") );

  add ( new GraphFactory::Advocate<StokesCylindrical>
	("Scyl", "Stokes; vector in cylindrical") );

  add ( new GraphFactory::Advocate<StokesSpherical>
	("Ssph", "Stokes; vector in spherical") );
}

//! Return a new instance of the named plotter
Pulsar::Graph* Pulsar::GraphFactory::construct (std::string name)
{
  for (unsigned i=0; i < agents.size(); i++)
    if (name == agents[i]->get_name())
      return agents[i]->construct();

  throw Error (InvalidParam, "Pulsar::GraphFactory::construct",
	       "no Graph named " + name);
}
    
//! Return the name of the ith agent
std::string Pulsar::GraphFactory::get_name (unsigned i)
{
  return agents[i]->get_name();
}

//! Return the name of the ith agent
std::string Pulsar::GraphFactory::get_description (unsigned i)
{
  return agents[i]->get_description();
}
