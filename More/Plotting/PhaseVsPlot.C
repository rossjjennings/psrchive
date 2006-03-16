#include "Pulsar/PhaseVsPlot.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

#include <cpgplot.h>

#include <values.h>
#include <algorithm>

Pulsar::PhaseVsPlot::PhaseVsPlot ()
{
  colour_map.set_name( pgplot::ColourMap::Heat );

  // default is to invert the axis tick marks
  get_frame()->get_x_axis()->set_pgbox_opt ("BCINTS");

  get_frame()->get_y_axis()->set_pgbox_opt ("BINTS");
  get_frame()->get_y_axis()->set_alternate (true);

  get_frame()->set_label_offset (1.0);

  // ensure that no labels are printed inside the frame
  get_frame()->get_label_below()->set_all (PlotLabel::unset);
}
 
TextInterface::Class* Pulsar::PhaseVsPlot::get_interface ()
{
  return new Interface (this);
}

//! Derived classes must draw in the current viewport
void Pulsar::PhaseVsPlot::draw (const Archive* data)
{
  colour_map.apply ();

  // Forget about the actual max and min, and draw in normalized coordinates

  float x_min = 0.0;
  float x_max = 1.0;
  get_frame()->get_x_scale()->PlotScale::set_minmax (x_min, x_max);
  get_frame()->get_x_scale()->PlotScale::get_range (data, x_min, x_max);

  float y_min = 0.0;
  float y_max = 1.0;
  get_frame()->get_y_scale()->PlotScale::set_minmax (y_min, y_max);
  get_frame()->get_y_scale()->PlotScale::get_range (data, y_min, y_max);

  cpgswin (x_min, x_max, y_min, y_max);

  // Fill the image data
  unsigned nbin = data->get_nbin();
  unsigned nrow = get_nrow (data);

  unsigned min_bin, max_bin;
  get_frame()->get_x_scale()->get_range (nbin, min_bin, max_bin);

  unsigned min_row, max_row;
  get_frame()->get_y_scale()->get_range (nrow, min_row, max_row);

  float min = MAXFLOAT;
  float max = MINFLOAT;

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

  // X = TR(0) + TR(1)*I + TR(2)*J
  // Y = TR(3) + TR(4)*I + TR(5)*J

  float x_res = 1.0/nbin;
  float y_res = 1.0/nrow;

  float trf[6] = { -0.5*x_res, x_res, 0.0,
		   -0.5*y_res, 0.0, y_res };

  
  get_z_scale()->set_minmax (min, max);
  get_z_scale()->get_range (data, min, max);

  cpgimag(&plotarray[0], nbin, nrow, 1, nbin, 1, nrow, min, max, trf);

  if (get_frame()->get_y_axis()->get_alternate())  {
    y_min *= nrow;
    y_max *= nrow;
    cpgswin (x_min, x_max, y_min, y_max);
    cpgbox (" ", 0.0, 0, "CMIST", 0.0, 0);
    cpgmtxt("R", 2.6, 0.5, 0.5, "Index");
  }

}
