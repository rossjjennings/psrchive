#include "Pulsar/PlotFactory.h"
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
Pulsar::PlotFactory::PlotFactory ()
{
  add ( new PlotFactory::Advocate<SinglePlotter>
	("flux", "Single plot of flux") );

  add ( new PlotFactory::Advocate<StokesPlotter>
	("stokes", "Stokes parameters") );

  add ( new PlotFactory::Advocate<PhaseVsFrequency>
	("freq", "Phase vs. frequency image of flux") );

  add ( new PlotFactory::Advocate<PhaseVsTime>
	("time", "Phase vs. time image of flux") );

  add ( new PlotFactory::Advocate<PosAngPlotter>
	("pa", "Position angle") );

  add ( new PlotFactory::Advocate<EllAngPlotter>
	("ell", "Ellipticity angle") );

  add ( new PlotFactory::Advocate<StokesCylindrical>
	("Scyl", "Stokes; vector in cylindrical") );

  add ( new PlotFactory::Advocate<StokesSpherical>
	("Ssph", "Stokes; vector in spherical") );
}

//! Return a new instance of the named plotter
Pulsar::Plot* Pulsar::PlotFactory::construct (std::string name)
{
  for (unsigned i=0; i < agents.size(); i++)
    if (name == agents[i]->get_name())
      return agents[i]->construct();

  throw Error (InvalidParam, "Pulsar::PlotFactory::construct",
	       "no Plot named " + name);
}
    
//! Return the name of the ith agent
std::string Pulsar::PlotFactory::get_name (unsigned i)
{
  return agents[i]->get_name();
}

//! Return the name of the ith agent
std::string Pulsar::PlotFactory::get_description (unsigned i)
{
  return agents[i]->get_description();
}
