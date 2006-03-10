#include "Pulsar/MultiProfile.h"
#include "Pulsar/MultiFrameTI.h"

#include <cpgplot.h>

TextInterface::Class* Pulsar::MultiProfile::get_frame_interface ()
{
  return new MultiFrameTI (&frames);
}

void Pulsar::MultiProfile::plot (const Archive* data)
{
  prepare (data);

  float x0, x1, y0, y1;
  cpgqvp (0, &x0, &x1, &y0, &y1);

  std::map< std::string, Reference::To<ProfilePlotter> >::iterator ptr;
  for (ptr = plotters.begin(); ptr != plotters.end(); ptr++) {
    ptr->second->get_frame()->focus();
    ptr->second->plot(data);
    // restore the viewport
    cpgsvp (x0,x1, y0,y1);
  }
}

//! Manage a plotter
void Pulsar::MultiProfile::manage (const std::string& name,
				   ProfilePlotter* plot)
{
  plot->set_frame( frames.manage (name, plot->get_frame()) );
  plotters[name] = plot;
}

//! Set the viewport of the named plotter
void Pulsar::MultiProfile::set_viewport (const std::string& name,
					 float x0, float x1,
					 float y0, float y1)
{
  PlotFrameSize* frame = frames.get_frame(name);
  frame->set_x_range( std::pair<float,float> (x0,x1) );
  frame->set_y_range( std::pair<float,float> (y0,y1) );
}
