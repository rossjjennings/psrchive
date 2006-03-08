#include "Pulsar/ProfilePlotter.h"
#include "Pulsar/ProfilePlotterTI.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include <cpgplot.h>

Pulsar::ProfilePlotter::ProfilePlotter ()
{
  yrange_set = false;
  y_min = y_max = 0;

  scale = Turns;
  origin_norm = 0;
}

Pulsar::ProfilePlotter::~ProfilePlotter ()
{
}

TextInterface::Class* Pulsar::ProfilePlotter::get_text_interface ()
{
  if (!text_interface)
    text_interface = new ProfilePlotterTI (this);
  return text_interface;
}

void Pulsar::ProfilePlotter::set_yrange (float min, float max)
{
  if (min >= max)
    throw Error (InvalidParam, "Pulsar::ProfilePlotter::set_yrange",
		 "ymin=%f >= ymax=%f", min, max);
  y_min = min;
  y_max = max;
  yrange_set = true;
}


//! Get the default label for the x axis
string Pulsar::ProfilePlotter::get_xlabel (const Archive*)
{
  switch (scale) {
  case Turns: return "Pulse Phase";
  case Degrees: return "Phase (deg.)";
  case Radians: return "Phase (rad.)";
  case Milliseconds: return "Time (ms)";
  }
}

//! Get the default label for the y axis
string Pulsar::ProfilePlotter::get_ylabel (const Archive*)
{
  return "";
}

/*!
  Plots the profile in the currently open pgplot device, using the current
  viewport.  The profile may be rotated, scaled, and zoomed.
*/
void Pulsar::ProfilePlotter::plot (const Archive* data)
{
  yrange_set = false;

  prepare (data);

  if (!yrange_set)
    throw Error (InvalidState, "Pulsar::ProfilePlotter::plot",
		 "range in y values not set after call to prepare");

  float y_diff = y_max - y_min;
  y_max = y_min + get_frame()->get_y_axis()->get_max_norm() * y_diff;
  y_min = y_min + get_frame()->get_y_axis()->get_min_norm() * y_diff;

  float x_max = get_frame()->get_x_axis()->get_max_norm();
  float x_min = get_frame()->get_x_axis()->get_min_norm();

  float y_space = (y_max - y_min) * get_frame()->get_y_axis()->get_buf_norm();
  y_min -= y_space;
  y_max += y_space;

  float x_space = (x_max - x_min) * get_frame()->get_x_axis()->get_buf_norm();
  x_min -= x_space;
  x_max += x_space;

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

}


ostream& Pulsar::operator << (ostream& os, ProfilePlotter::Scale scale)
{
  switch (scale) {
  case ProfilePlotter::Turns:
    return os << "turn";
  case ProfilePlotter::Degrees:
    return os << "deg";
  case ProfilePlotter::Radians:
    return os << "rad";
  case ProfilePlotter::Milliseconds:
    return os << "ms";
  }
}

istream& Pulsar::operator >> (istream& is, ProfilePlotter::Scale& scale)
{
  std::streampos pos = is.tellg();
  string unit;
  is >> unit;

  if (unit == "turn")
    scale = ProfilePlotter::Turns;
  else if (unit == "deg")
    scale = ProfilePlotter::Degrees;
  else if (unit == "rad")
    scale = ProfilePlotter::Radians;
  else if (unit == "ms")
    scale = ProfilePlotter::Milliseconds;
  else
    is.setstate(istream::failbit);

  return is;
}
