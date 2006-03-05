#include "Pulsar/PlotterFactory.h"
#include "Error.h"

//! Return a new instance of the named plotter
Pulsar::ProfilePlotter* Pulsar::PlotterFactory::construct (std::string name)
{
  for (unsigned i=0; i < agents.size(); i++)
    if (name == agents[i]->get_name())
      return agents[i]->construct();

  throw Error (InvalidParam, "Pulsar::PlotterFactory::construct",
	       "no Plotter named " + name);
}
    
//! Return the name of the ith agent
std::string Pulsar::PlotterFactory::get_name (unsigned i)
{
  return agents[i]->get_name();
}

//! Return the name of the ith agent
std::string Pulsar::PlotterFactory::get_description (unsigned i)
{
  return agents[i]->get_description();
}

