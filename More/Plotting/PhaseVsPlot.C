#include "Pulsar/PhaseVsPlotTI.h"
#include "Pulsar/PlotFrame.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

#include <cpgplot.h>

#include <algorithm>

Pulsar::PhaseVsPlot::PhaseVsPlot ()
{
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
  return new PhaseVsPlotTI (this);
}

//! Derived classes must draw in the current viewport
void Pulsar::PhaseVsPlot::draw (const Archive* archive)
{
  // Forget about what the PhasePlot base class knows about max and min

  float x_min = 0.0;
  float x_max = 1.0;
  get_frame()->get_x_zoom()->get_range (x_min, x_max);

  float y_min = 0.0;
  float y_max = 1.0;
  get_frame()->get_y_zoom()->get_range (y_min, y_max);

  cpgswin (x_min, x_max, y_min, y_max);

  // Fill the image data
  unsigned nbin = archive->get_nbin();
  unsigned nrow = get_nrow (archive);

  vector<float> plotarray (nbin * nrow);
  for (unsigned irow = 0; irow < nrow; irow++) {
    vector<float> amps  = get_Profile (archive, irow) -> get_weighted_amps();
    for (int ibin=0; ibin<nbin; ibin++)
      plotarray[irow*nbin + ibin] = amps[ibin];
  }

  // X = TR(0) + TR(1)*I + TR(2)*J
  // Y = TR(3) + TR(4)*I + TR(5)*J

  float x_res = 1.0/nbin;
  float y_res = 1.0/nrow;

  float trf[6] = { -0.5*x_res, x_res, 0.0,
		   -0.5*y_res, 0.0, y_res };

  float min = * std::min_element (plotarray.begin(), plotarray.end());
  float max = * std::max_element (plotarray.begin(), plotarray.end());
  
  get_z_zoom()->get_range (min, max);

  cpgimag(&plotarray[0], nbin, nrow, 1, nbin, 1, nrow, min, max, trf);

  if (get_frame()->get_y_axis()->get_alternate())  {
    y_min *= nrow;
    y_max *= nrow;
    cpgswin (x_min, x_max, y_min, y_max);
    cpgbox (" ", 0.0, 0, "CMIST", 0.0, 0);
    cpgmtxt("R", 2.6, 0.5, 0.5, "Index");
  }
  else
    get_frame()->get_y_axis()->add_pgbox_opt('C');

}
