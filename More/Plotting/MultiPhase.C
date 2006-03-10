#include "Pulsar/MultiProfile.h"
//#include "Pulsar/MultiProfileTI.h"

#include <cpgplot.h>

void Pulsar::MultiProfile::plot (const Archive* data)
{
  std::map< std::string, Reference::To<ProfilePlotter> >::iterator ptr;
  for (ptr = plotters.begin(); ptr != plotters.end(); ptr++)
    ptr->second->plot(data);
}

//! Manage a plotter
void Pulsar::MultiProfile::manage (const std::string& name,
				   ProfilePlotter* plot)
{
  plot->set_frame( frames.manage (name, plot->get_frame()) );
  plotters[name] = plot;
}

//! Set the viewport of the named plotter
void
Pulsar::MultiProfile::set_viewport (const std::string& name,
				    std::pair<float,float> bottom_left,
				    std::pair<float,float> top_right)
{
  PlotFrameSize* frame = frames.get_frame(name);
  frame->set_bottom_left( bottom_left );
  frame->set_top_right( top_right );
}
