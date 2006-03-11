#include "Pulsar/PhasePlotTI.h"
#include "Pulsar/PlotFrameTI.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include <cpgplot.h>

Pulsar::PhasePlot::PhasePlot ()
{
  frame = new PlotFrame;

  yrange_set = false;
  y_min = y_max = 0;

  scale = Turns;
  origin_norm = 0;
}

Pulsar::PhasePlot::~PhasePlot ()
{
}

TextInterface::Class* Pulsar::PhasePlot::get_interface ()
{
  return new PhasePlotTI (this);
}

//! Get the text interface to the frame attributes
TextInterface::Class* Pulsar::PhasePlot::get_frame_interface ()
{
  return new PlotFrameTI (get_frame());
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
  switch (scale) {
  case Turns: return "Pulse Phase";
  case Degrees: return "Phase (deg.)";
  case Radians: return "Phase (rad.)";
  case Milliseconds: return "Time (ms)";
  }
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

  if (scale == Milliseconds)
    x_scale = data->get_Integration(0)->get_folding_period() * 1e3;

  else if (scale == Radians)
    x_scale = 2.0 * M_PI;

  else if (scale == Degrees)
    x_scale = 180.0;

  x_min += origin_norm;
  x_max += origin_norm;

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


ostream& Pulsar::operator << (ostream& os, PhasePlot::Scale scale)
{
  switch (scale) {
  case PhasePlot::Turns:
    return os << "turn";
  case PhasePlot::Degrees:
    return os << "deg";
  case PhasePlot::Radians:
    return os << "rad";
  case PhasePlot::Milliseconds:
    return os << "ms";
  }
}

istream& Pulsar::operator >> (istream& is, PhasePlot::Scale& scale)
{
  std::streampos pos = is.tellg();
  string unit;
  is >> unit;

  if (unit == "turn")
    scale = PhasePlot::Turns;
  else if (unit == "deg")
    scale = PhasePlot::Degrees;
  else if (unit == "rad")
    scale = PhasePlot::Radians;
  else if (unit == "ms")
    scale = PhasePlot::Milliseconds;
  else
    is.setstate(istream::failbit);

  return is;
}
