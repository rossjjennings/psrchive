/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/MultiPlot.h"
#include "Pulsar/FramedPlot.h"
#include "Pulsar/MultiFrame.h"
#include "pairutil.h"

#include <cpgplot.h>

using namespace std;

Pulsar::MultiPlot::MultiPlot ()
{
}

TextInterface::Parser* Pulsar::MultiPlot::get_frame_interface ()
{
  return new MultiFrame::Interface (&frames);
}

Pulsar::PlotAttributes* Pulsar::MultiPlot::get_attributes ()
{
  return &frames;
}

void Pulsar::MultiPlot::prepare (const Archive* data)
{
  cerr << "Pulsar::MultiPlot::prepare nplot=" << plots.size() << endl;

  std::map< std::string, Reference::To<FramedPlot> >::iterator ptr;
  for (ptr = plots.begin(); ptr != plots.end(); ptr++)
  {
    FramedPlot* plot = ptr->second;
    plot->prepare (data);
  }
}

void Pulsar::MultiPlot::plot (const Archive* data)
{
  if (verbose)
    cerr << "Pulsar::MultiPlot::plot call prepare" << endl;

  prepare (data);

  if (verbose)
    cerr << "Pulsar::MultiPlot::plot done prepare" << endl;
  
  std::map< std::string, Reference::To<FramedPlot> >::iterator ptr;
  for (ptr = plots.begin(); ptr != plots.end(); ptr++)
  {

    FramedPlot* plot = ptr->second;
    PlotFrame* frame = plot->get_frame();

    // set the viewport of the frame
    std::pair<float,float> xvp, yvp;
    set_viewport (frame, xvp, yvp);

    Error* to_throw = 0;

    try {
      // prepare the plot
      prepare (plot);

      // plot
      plot->plot(data);
    }
    catch (Error& error)
    {
      error += "Pulsar::MultiPlot::plot";
      error << " (in " << ptr->first << ")";
      to_throw = new Error (error);
    }

    // restore the viewport of the frame
    frame->get_x_scale(true)->set_viewport( xvp );
    frame->get_y_scale(true)->set_viewport( yvp );

    // this could cause a memory leak
    if (to_throw)
      throw *to_throw;

  }

  if (verbose)
    cerr << "Pulsar::MultiPlot::plot focus" << endl;
  get_frame()->focus (data);

  if (verbose)
    cerr << "Pulsar::MultiPlot::plot decorate frame" << endl;
  get_frame()->decorate (data);
}

void Pulsar::MultiPlot::set_viewport (PlotFrame* frame, 
				      std::pair<float,float>& sub_xvp,
				      std::pair<float,float>& sub_yvp)
{
  // get the total viewport allocated to the multiple plots
  std::pair<float,float> xvp = frames.get_x_edge()->get_viewport();
  std::pair<float,float> yvp = frames.get_y_edge()->get_viewport();

#if _DEBUG
  cerr << "Pulsar::MultiPlot::set_viewport total"
    " x=" << xvp << " y=" << yvp << endl;
#endif

  // get the fraction allocated to this sub-plot
  sub_xvp = frame->get_x_scale(true)->get_viewport();
  sub_yvp = frame->get_y_scale(true)->get_viewport();

#if _DEBUG
  cerr << "Pulsar::MultiPlot::set_viewport sub"
    " x=" << sub_xvp << " y=" << sub_yvp << endl;
#endif

  // calculate the total viewport allocated to this sub-plot
  stretch (sub_xvp, xvp);
  stretch (sub_yvp, yvp);

  // set the viewport accordingly
  frame->get_x_scale(true)->set_viewport( xvp );
  frame->get_y_scale(true)->set_viewport( yvp );
}

void Pulsar::MultiPlot::manage (Plot* plot)
{
  FramedPlot* framed = dynamic_cast<FramedPlot*> (plot);
  if (!framed)
    throw Error (InvalidParam, "Pulsar::MultiPlot::manage",
		 "Plot is not a FramedPlot");

  string name = tostring(plots.size());
  manage (name, framed);
  
  if (frames.has_shared_x_scale())
  {
    if (verbose)
      cerr << "Pulsar::MultiPlot::manage stacking vertically" << endl;

    unsigned counter = 0;

    double y_each = 1.0 / plots.size();

    std::map< std::string, Reference::To<FramedPlot> >::iterator ptr;
    for (ptr = plots.begin(); ptr != plots.end(); ptr++)
    {
      FramedPlot* plot = ptr->second;
      PlotFrame* frame = plot->get_frame();

      if (counter > 0)
      {
	// remove the above frame labels
	frame->get_label_above()->set_all ("");

	// remove the below frame labels
	frame->get_label_below()->set_all ("");
      }

      frame->set_viewport (0,1, 1.0-(counter+1)*y_each, 1.0-counter*y_each);

      counter ++;

      if (plots.size() > 1 && counter < plots.size())
      {
	// remove the x label
	frame->get_x_axis()->set_label(" ");

	// remove the x enumeration
	frame->get_x_axis()->rem_opt('N');
      }
    }
  }
}

//! Manage a plot
void Pulsar::MultiPlot::manage (const std::string& name, FramedPlot* plot)
{
  if (verbose)
    cerr << "Pulsar::MultiPlot::manage name=" << name << endl;

  frames.manage (name, plot->get_frame());

  plot->get_frame()->get_label_above()->set_all( PlotLabel::unset );

  if (frames.has_shared_x_scale())
  {
    if (verbose)
      cerr << "Pulsar::MultiPlot::manage shared x_scale" << endl;
    plot->get_frame()->set_x_scale( frames.get_shared_x_scale() );
  }

  if (frames.has_shared_y_scale())
  {
    if (verbose)
      cerr << "Pulsar::MultiPlot::manage shared y_scale" << endl;
    plot->get_frame()->set_y_scale( frames.get_shared_y_scale() );
  }

  plots[name] = plot;
}

void Pulsar::MultiPlot::unmanage (FramedPlot* plot)
{
  std::map< std::string, Reference::To<FramedPlot> >::iterator ptr;
  for (ptr = plots.begin(); ptr != plots.end(); ptr++)
    if (ptr->second.ptr() == plot) {
      plots.erase (ptr);
      return;
    }
}

#include "Pulsar/MultiPhase.h"

#include "Pulsar/MultiFrequency.h"
#include "Pulsar/FrequencyPlot.h"

Pulsar::MultiPlot* Pulsar::MultiPlot::factory (Plot* plot)
{
  if (dynamic_cast<HasPhaseScale*>(plot))
    return new MultiPhase;

  if (dynamic_cast<FrequencyPlot*>(plot))
    return new MultiFrequency;

  throw Error (InvalidParam, "Pulsar::MultiPlot::factory",
	       "Plot is neither PhasePlot nor FrequencyPlot");
}
