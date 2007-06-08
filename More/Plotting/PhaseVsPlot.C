/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseVsPlot.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

#include <cpgplot.h>

#include <algorithm>

#include <float.h>

using namespace std;

Pulsar::PhaseVsPlot::PhaseVsPlot ()
{
  colour_map.set_name( pgplot::ColourMap::Heat );

  // default is to invert the axis tick marks
  get_frame()->get_x_axis()->set_pgbox_opt ("BCINTS");

  get_frame()->get_y_axis()->set_pgbox_opt ("BINTS");
  get_frame()->get_y_axis()->set_alternate (true);

  get_frame()->get_label_above()->set_offset (1.0);

  // ensure that no labels are printed inside the frame
  get_frame()->get_label_below()->set_all (PlotLabel::unset);

  style = "image";
}
 
TextInterface::Class* Pulsar::PhaseVsPlot::get_interface ()
{
  return new Interface (this);
}

void Pulsar::PhaseVsPlot::set_style (const string& s)
{
  if (s != "image" && s != "line")
    throw Error (InvalidParam, "Pulsar::PhaseVsPlot::set_style",
		 "invalid style '" + s + "'");
  style = s;
}

//! Derived classes must draw in the current viewport
void Pulsar::PhaseVsPlot::draw (const Archive* data)
{
  colour_map.apply ();

  float x_min = 0.0;
  float x_max = 1.0;
  get_frame()->get_x_scale()->PlotScale::get_minmax (x_min, x_max);

  float y_min = 0.0;
  float y_max = 1.0;
  get_frame()->get_y_scale()->PlotScale::get_minmax (y_min, y_max);

  // Fill the image data
  unsigned nbin = data->get_nbin();
  unsigned nrow = get_nrow (data);

  unsigned min_bin, max_bin;
  get_frame()->get_x_scale()->get_range (nbin, min_bin, max_bin);

  unsigned min_row, max_row;
  get_frame()->get_y_scale()->get_range (nrow, min_row, max_row);

  float min = FLT_MAX;
  float max = FLT_MIN;

  vector<float> plotarray (nbin * nrow);
  for (unsigned irow = 0; irow < nrow; irow++) {
    vector<float> amps  = get_Profile (data, irow) -> get_weighted_amps();
    for (unsigned ibin=0; ibin<nbin; ibin++)
      plotarray[irow*nbin + ibin] = amps[ibin];

    if (irow < min_row || irow >= max_row)
      continue;

    min = std::min (min, *std::min_element (amps.begin()+min_bin,
					    amps.begin()+max_bin) );

    max = std::max (max, *std::max_element (amps.begin()+min_bin,
					    amps.begin()+max_bin) );

  }

  float x_res = (x_max-x_min)/nbin;
  float y_res = (y_max-y_min)/nrow;
    
  if (style == "image") {

    get_z_scale()->set_minmax (min, max);
    get_z_scale()->get_range (min, max);

    // X = TR(0) + TR(1)*I + TR(2)*J
    // Y = TR(3) + TR(4)*I + TR(5)*J
    
    float trf[6] = { x_min-0.5*x_res, x_res, 0.0,
		     y_min-0.5*y_res, 0.0, y_res };
    
    cpgimag(&plotarray[0], nbin, nrow, 1, nbin, 1, nrow, min, max, trf);

  }
  else if (style == "line") {

    get_z_scale()->set_minmax (0, max);
    get_z_scale()->get_range (min, max);

    vector<float> xaxis;
    get_scale()->get_ordinates (data, xaxis);

    float yscale = y_res/max;

    for (unsigned irow = min_row; irow < max_row; irow++) {
      bool all_zero = true;
      for (unsigned ibin=0; ibin<nbin; ibin++) {
	float amp = plotarray[irow*nbin + ibin];
	if (amp != 0.0)
	  all_zero = false;
	plotarray[irow*nbin + ibin] = amp * yscale + y_min + y_res * irow;
      }
      if (!all_zero)
	cpgline (nbin, &xaxis[0], &plotarray[irow*nbin]);
    }

  }

  if (get_frame()->get_y_axis()->get_alternate()) {

    float min, max;
    get_frame()->get_y_scale()->PlotScale::get_range (min, max);

    float length = y_max - y_min;

    min = (min - y_min) / length;
    max = (max - y_min) / length;

    min *= nrow;
    max *= nrow;

    cpgswin (x_min, x_max, min, max);

    cpgbox (" ", 0.0, 0, "CMIST", 0.0, 0);
    cpgmtxt("R", 2.6, 0.5, 0.5, "Index");
  }

}
