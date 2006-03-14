#include "Pulsar/PhasePlotTI.h"
#include "Pulsar/PlotFrame.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include <cpgplot.h>

Pulsar::PhasePlot::PhasePlot ()
{
  get_frame()->set_x_zoom( new PhaseScale );

  yrange_set = false;
  y_min = y_max = 0;
}

Pulsar::PhasePlot::~PhasePlot ()
{
}

TextInterface::Class* Pulsar::PhasePlot::get_interface ()
{
  return new PhasePlotTI (this);
}

void Pulsar::PhasePlot::set_yrange (float min, float max)
{
  y_min = min;
  y_max = max;
  yrange_set = true;
}


//! Get the default label for the x axis
string Pulsar::PhasePlot::get_xlabel (const Archive*)
{
  return get_scale()->get_label();
}

//! Get the default label for the y axis
string Pulsar::PhasePlot::get_ylabel (const Archive*)
{
  return "";
}

void Pulsar::PhasePlot::get_range_bin (const Archive* data, 
				       unsigned& min, unsigned& max)
{
  float x_min = 0.0;
  float x_max = data->get_nbin();
  get_frame()->get_x_zoom()->get_range (x_min, x_max);
  min = (unsigned) x_min;
  max = (unsigned) x_max;
}

/*!
  Plots the profile in the currently open pgplot device, using the current
  viewport.  The profile may be rotated, scaled, and zoomed.
*/
void Pulsar::PhasePlot::plot (const Archive* data)
{
  yrange_set = false;

  prepare (data);

  if (!yrange_set)
    throw Error (InvalidState, "Pulsar::PhasePlot::plot",
		 "range in y values not set after call to prepare");

  float x_min = 0.0;
  float x_max = 1.0;

  get_frame()->get_y_zoom()->get_range (y_min, y_max);
  get_frame()->get_x_zoom()->get_range (x_min, x_max);

  cpgswin (x_min, x_max, y_min, y_max);

  phases.resize (data->get_nbin());
  for (unsigned ibin = 0; ibin < phases.size(); ibin++)
    phases[ibin] = (float(ibin) + 0.5) / phases.size();

  draw (data);

  cpgsls (1);
  cpgsci (1);

  float x_scale = 1.0;

  if (get_scale()->get_scale() == PhaseScale::Milliseconds)
    x_scale = data->get_Integration(0)->get_folding_period() * 1e3;

  else if (get_scale()->get_scale() == PhaseScale::Radians)
    x_scale = 2.0 * M_PI;

  else if (get_scale()->get_scale() == PhaseScale::Degrees)
    x_scale = 180.0;

  x_min += get_scale()->get_origin_norm();
  x_max += get_scale()->get_origin_norm();

  x_min *= x_scale;
  x_max *= x_scale;

  cpgswin (x_min, x_max, y_min, y_max);

  cpgbox( get_frame()->get_x_axis()->get_pgbox_opt().c_str(), 0.0, 0,
	  get_frame()->get_y_axis()->get_pgbox_opt().c_str(), 0.0, 0 );

  string ylabel = get_frame()->get_y_axis()->get_label();
  if (ylabel == PlotLabel::unset)
    ylabel = get_ylabel (data);

  string xlabel = get_frame()->get_x_axis()->get_label();
  if (xlabel == PlotLabel::unset)
    xlabel = get_xlabel (data);

  cpgmtxt ("L",2.5,.5,.5, ylabel.c_str());
  cpgmtxt ("B",2.5,.5,.5, xlabel.c_str());

  get_frame()->decorate(data);
}

//! Get the scale
Pulsar::PhaseScale* Pulsar::PhasePlot::get_scale ()
{
  PhaseScale* scale = dynamic_cast<PhaseScale*>( get_frame()->get_x_zoom() );
  if (!scale)
    throw Error (InvalidState, "Pulsar::PhasePlot::get_scale",
		 "x scale is not a PhaseScale");
  return scale;
}
