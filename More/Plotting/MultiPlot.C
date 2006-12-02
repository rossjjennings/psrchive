/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/MultiPlot.h"
#include "Pulsar/MultiFrame.h"
#include "iopair.h"

#include <cpgplot.h>

using namespace std;

Pulsar::MultiPlot::MultiPlot ()
{
  set_attributes( &frames );
}

TextInterface::Class* Pulsar::MultiPlot::get_frame_interface ()
{
  return new MultiFrame::Interface (&frames);
}

void Pulsar::MultiPlot::plot (const Archive* data)
{
  prepare (data);

  std::map< std::string, Reference::To<Plot> >::iterator ptr;
  for (ptr = plots.begin(); ptr != plots.end(); ptr++) {

    Plot* plot = ptr->second;
    PlotFrame* frame = plot->get_frame();

    // set the viewport of the frame
    std::pair<float,float> xvp, yvp;
    set_viewport (frame, xvp, yvp);

    // prepare the plot
    prepare (plot);

    // plot
    plot->plot(data);

    // restore the viewport of the frame
    frame->get_x_scale()->set_viewport( xvp );
    frame->get_y_scale()->set_viewport( yvp );

  }
}

void Pulsar::MultiPlot::set_viewport (PlotFrame* frame, 
				      std::pair<float,float>& sub_xvp,
				      std::pair<float,float>& sub_yvp)
{
  // get the total viewport allocated to the multiple plots
  std::pair<float,float> xvp = frames.get_x_edge()->get_viewport();
  std::pair<float,float> yvp = frames.get_y_edge()->get_viewport();

  // get the fraction allocated to this sub-plot
  sub_xvp = frame->get_x_scale()->get_viewport();
  sub_yvp = frame->get_y_scale()->get_viewport();

  // calculate the total viewport allocated to this sub-plot
  stretch (sub_xvp, xvp);
  stretch (sub_yvp, yvp);

  // set the viewport accordingly
  frame->get_x_scale()->set_viewport( xvp );
  frame->get_y_scale()->set_viewport( yvp );
}

//! Manage a plot
void Pulsar::MultiPlot::manage (const std::string& name, Plot* plot)
{
  frames.manage (name, plot->get_frame());

  if (frames.has_shared_x_scale())
    plot->get_frame()->set_x_scale( frames.get_shared_x_scale() );

  if (frames.has_shared_y_scale())
    plot->get_frame()->set_y_scale( frames.get_shared_y_scale() );

  plots[name] = plot;
}

