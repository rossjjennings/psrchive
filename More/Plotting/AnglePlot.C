/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/AnglePlot.h"
#include "Pulsar/AnglePlot.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"
#include "templates.h"

#include <cpgplot.h>
#include <algorithm>

using namespace std;

Pulsar::AnglePlot::AnglePlot()
{
  marker = ErrorBar;
  threshold = 4.0;
  span = 0.0;

  isubint = 0;
  ichan = 0;

  get_frame()->get_y_axis()->set_tick( 60.0 );
  get_frame()->get_y_axis()->set_nsub( 3 );
}

TextInterface::Class* Pulsar::AnglePlot::get_interface ()
{
  return new Interface (this);
}

void Pulsar::AnglePlot::prepare (const Archive* data)
{
  get_angles (data);

  if (angles.size() != data->get_nbin())
    throw Error (InvalidState, "Pulsar::AnglePlot::prepare",
                 "angles vector size=%u != nbin=%u",
                 angles.size(), data->get_nbin());

  float min = 0;
  float max = 0;

  if (span)
  {
    // keep pgplot from drawing the 90 or 180 at the edge
    float half = 0.5 * span - 0.0001;
    min = -half;
    max = half;
  }
  else
  {
    unsigned i_min, i_max;
    get_scale()->get_indeces (data, i_min, i_max);
    Estimate<double> e_min, e_max;
    cyclic_minmax (angles, i_min, i_max, e_min, e_max);
    min = e_min.get_value();
    max = e_max.get_value();
  }

  get_frame()->get_y_scale()->set_minmax (min, max);
}

void Pulsar::AnglePlot::draw (const Archive *data)
{
  std::vector<float> phases;
  get_scale()->get_ordinates (data, phases);

  pair<float,float> range = get_scale()->get_range_norm();

  int range_start = int(floor(range.first));
  int range_end = int(ceil(range.second));

  for( int range = range_start; range < range_end; range ++ )
  {
    float yoff = 0;
    unsigned times = 1;

    float xoff = float(range) * get_scale()->get_scale(data);

    if (span)
    {
      yoff = -span;
      times = 3;
    }

    bool err1 = false;
    float terminal = 0.0;

    if (marker & ErrorBar) {
      terminal = 1.0;
      err1 = true;
    }

    if (marker & ErrorTick) {
      terminal = 0.0;
      err1 = true;
    }

    for (unsigned ioff=0; ioff < times; ioff++)
    {
      for (unsigned ibin=0; ibin < phases.size(); ibin++)
        if (angles[ibin].get_variance() != 0) {
          if (err1)
            cpgerr1 (6, phases[ibin]+xoff, angles[ibin].get_value() + yoff,
                     angles[ibin].get_error(), terminal);
	  if (marker & Dot)
            cpgpt1 (phases[ibin]+xoff, angles[ibin].get_value() + yoff, 17);
	}

      yoff += span;

    }

  }

}

//! Return the label for the y-axis
std::string Pulsar::AnglePlot::get_flux_label (const Archive* data)
{
  return "P.A. (deg.)";
}

void Pulsar::AnglePlot::set_marker (const std::string& code)
{
  if (code == "dot" | marker & Dot)
    marker = Dot;
  else
    marker = 0;

  if (code == "bar")
    marker |= ErrorBar;

  if (code == "tick")
    marker |= ErrorTick;
}
  
std::string Pulsar::AnglePlot::get_marker () const
{
  string retval;

  if (marker & Dot)
    retval = "dot";

  string error;

  if (marker & ErrorBar)
    error = "bar";

  if (marker & ErrorTick)
    error = "tick";

  retval += (marker & Dot)?"+":"" + error;

  return retval;
}


