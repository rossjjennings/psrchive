#include "Pulsar/MultiProfile.h"
#include "Pulsar/MultiFrameTI.h"

#include <cpgplot.h>

Pulsar::MultiProfile::MultiProfile ()
{
  frames.set_shared_x_axis (get_frame()->get_x_axis());
}

TextInterface::Class* Pulsar::MultiProfile::get_frame_interface ()
{
  return new MultiFrameTI (&frames);
}

void Pulsar::MultiProfile::plot (const Archive* data)
{
  prepare (data);

  float x0, x1, y0, y1;
  cpgqvp (0, &x0, &x1, &y0, &y1);

  // the x-range
  std::pair<float,float> x_range = get_frame()->get_x_axis()->get_range_norm();

  std::map< std::string, Reference::To<ProfilePlotter> >::iterator ptr;
  for (ptr = plotters.begin(); ptr != plotters.end(); ptr++) {

    ProfilePlotter* plot = ptr->second;
    PlotFrame* frame = plot->get_frame();

    // ensure that all plots have the same x-axis range
    frame->get_x_axis()->set_range_norm( x_range );

    // set the viewport
    frame->focus();

    // plot
    plot->set_scale (get_scale());
    plot->set_origin_norm (get_origin_norm());
    plot->plot(data);

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
