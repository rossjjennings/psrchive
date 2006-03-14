#include "Pulsar/MultiPlot.h"
#include "Pulsar/MultiFrameTI.h"

#include <cpgplot.h>

TextInterface::Class* Pulsar::MultiPlot::get_frame_interface ()
{
  return new MultiFrameTI (&frames);
}

void Pulsar::MultiPlot::plot (const Archive* data)
{
  float x0, x1, y0, y1;
  cpgqvp (0, &x0, &x1, &y0, &y1);

  prepare (data);

  std::map< std::string, Reference::To<Plot> >::iterator ptr;
  for (ptr = plots.begin(); ptr != plots.end(); ptr++) {

    Plot* plot = ptr->second;

    // prepare the plot
    prepare (plot);

    // set the viewport
    plot->get_frame()->focus();

    // plot
    plot->plot(data);

    // restore the viewport
    cpgsvp (x0,x1, y0,y1);

  }
}

//! Manage a plot
void Pulsar::MultiPlot::manage (const std::string& name, Plot* plot)
{
  plot->set_frame( frames.manage (name, plot->get_frame()) );

  if (frames.has_shared_x_scale())
    plot->get_frame()->set_x_scale( frames.get_shared_x_scale() );

  plots[name] = plot;
}

//! Set the viewport of the named plot
void Pulsar::MultiPlot::set_viewport (const std::string& name,
				      float x0, float x1,
				      float y0, float y1)
{
  PlotFrameSize* frame = frames.get_frame(name);
  frame->set_x_range( std::pair<float,float> (x0,x1) );
  frame->set_y_range( std::pair<float,float> (y0,y1) );
}
