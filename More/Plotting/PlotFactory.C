#include "Pulsar/PlotFactory.h"
#include "Pulsar/ProfilePlot.h"
#include "Pulsar/StokesPlot.h"
#include "Pulsar/PhaseVsFrequency.h"
#include "Pulsar/PhaseVsTime.h"
#include "Pulsar/PosAngPlot.h"
#include "Pulsar/EllAngPlot.h"
#include "Pulsar/StokesCylindrical.h"
#include "Pulsar/StokesSpherical.h"
#include "Pulsar/SpectrumPlot.h"
#include "Error.h"

// Add plots here
Pulsar::PlotFactory::PlotFactory ()
{
  add ( new PlotFactory::Advocate<ProfilePlot>
	("flux", "Single plot of flux") );

  add ( new PlotFactory::Advocate<StokesPlot>
	("stokes", "Stokes parameters") );

  add ( new PlotFactory::Advocate<PhaseVsFrequency>
	("freq", "Phase vs. frequency image of flux") );

  add ( new PlotFactory::Advocate<PhaseVsTime>
	("time", "Phase vs. time image of flux") );

  add ( new PlotFactory::Advocate<PosAngPlot>
	("pa", "Position angle") );

  add ( new PlotFactory::Advocate<EllAngPlot>
	("ell", "Ellipticity angle") );

  add ( new PlotFactory::Advocate<StokesCylindrical>
	("Scyl", "Stokes; vector in cylindrical") );

  add ( new PlotFactory::Advocate<StokesSpherical>
	("Ssph", "Stokes; vector in spherical") );

  add ( new PlotFactory::Advocate<SpectrumPlot>
	("psd", "Total power spectrum") );
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
