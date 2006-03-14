#include "Pulsar/PhasePlotTI.h"
#include "Pulsar/PlotFrame.h"

#include "Pulsar/Archive.h"

#include <cpgplot.h>

Pulsar::PhasePlot::PhasePlot ()
{
  get_frame()->set_x_scale( new PhaseScale );

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

/*!
  Plots the profile in the currently open pgplot device, using the current
  viewport.  The profile may be rotated, scaled, and scaleed.
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

  get_frame()->get_y_scale()->get_range (y_min, y_max);
  get_frame()->get_x_scale()->get_range (x_min, x_max);

  cpgswin (x_min, x_max, y_min, y_max);

  phases.resize (data->get_nbin());
  for (unsigned ibin = 0; ibin < phases.size(); ibin++)
    phases[ibin] = (float(ibin) + 0.5) / phases.size();

  draw (data);

  cpgsls (1);
  cpgsci (1);

  get_scale()->get_range_units (data, x_min, x_max);

  cpgswin (x_min, x_max, y_min, y_max);

  get_frame()->draw_axes();

  get_frame()->label_axes( get_xlabel (data), get_ylabel (data) );

  get_frame()->decorate(data);
}

//! Get the scale
Pulsar::PhaseScale* Pulsar::PhasePlot::get_scale ()
{
  PhaseScale* scale = dynamic_cast<PhaseScale*>( get_frame()->get_x_scale() );
  if (!scale)
    throw Error (InvalidState, "Pulsar::PhasePlot::get_scale",
		 "x scale is not a PhaseScale");
  return scale;
}
