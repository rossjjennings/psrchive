#include "Pulsar/CalibratorPlotter.h"
#include "Pulsar/Calibrator.h"
#include "Pulsar/Archive.h"

#include "EstimatePlotter.h"

#include <cpgplot.h>

bool Pulsar::CalibratorPlotter::verbose = false;

Pulsar::CalibratorPlotter::CalibratorPlotter ()
{
  npanel = 3;
  use_colour = true;
}

Pulsar::CalibratorPlotter::~CalibratorPlotter ()
{
}

//! PGPLOT the calibrator model parameters as a function of frequency
void Pulsar::CalibratorPlotter::plot (const Calibrator* calibrator)
{ try {
  if (!calibrator)
    return;

  if (verbose)
    cerr << "Pulsar::CalibratorPlotter::plot" << endl;

  plot( calibrator->get_Info (), calibrator->get_nchan (),
	calibrator->get_Archive()->get_centre_frequency(),
	calibrator->get_Archive()->get_bandwidth() );

}
catch (Error& error) {
  throw error += "Pulsar::CalibratorPlotter::plot(Calibrator*)";
}
}

void Pulsar::CalibratorPlotter::plot (const Calibrator::Info* info,
				      unsigned nchan, double cfreq, double bw)
{ try {

  if (!info)
    return;
  
  Reference::To<const Calibrator::Info> manage = info;

  if (nchan == 0) {
    cerr << "Pulsar::CalibratorPlotter::plot no points to plot" << endl;
    return;
  }

  if (verbose)
    cerr << "Pulsar::CalibratorPlotter::plot nchan = " << nchan << endl;

  float xmin, xmax, ymin, ymax;
  cpgqvp (0, &xmin, &xmax, &ymin, &ymax);

  float ybottom = ymin;
  float yrange = ymax - ymin;
  float yspace = 0.1 * yrange;
  float yheight = (yrange - yspace) / float(npanel);
  float ybetween = 0;
  if (npanel > 1)
    ybetween = yspace / float(npanel -1);

  cpgsci(1);
  cpgslw(1);
  cpgsch(1);

  // the plotting class
  EstimatePlotter plotter;

  if (bw < 0.0)
    bw *= -1.0;

  plotter.set_xrange (cfreq-0.5*bw, cfreq+0.5*bw);

  // don't plot points with zero variance
  plotter.set_minimum_error (0.0);

  // the data to be plotted
  vector< Estimate<float> > data (nchan);

  // ////////////////////////////////////////////////////////////////////

  string xaxis;
  string xlabel;

  for (unsigned iplot=0; iplot < info->get_nclass(); iplot++) {

    if (iplot % npanel == 0) {

      xaxis = "bcnst";
      xlabel = "Frequency (MHz)";

      if (iplot)
	cpgpage ();

      ybottom = ymin;

      unsigned to_plot = info->get_nclass() - iplot;

      if (to_plot < npanel)
	ybottom += 0.5 * (ybetween + yheight) * float(npanel - to_plot);

    }
    else {
      xaxis = "bcst";
      xlabel = "";
    }

    plotter.clear ();

    unsigned nparam = info->get_nparam( iplot );
    float yscale = info->get_scale( iplot );

    unsigned iparam = 0;

    for (iparam=0; iparam<nparam; iparam++) {

      for (unsigned ichan=0; ichan<nchan; ichan++)
	data[ichan] = yscale * info->get_param (ichan, iplot, iparam);

      plotter.add_plot (data);

    }

    cpgsvp (xmin, xmax, ybottom, ybottom + yheight);


    for (iparam=0; iparam<nparam; iparam++) {

      if (use_colour)
	cpgsci ( info->get_colour_index(iplot, iparam) );
      else
	plotter.set_graph_marker ( info->get_graph_marker(iplot, iparam) );

      plotter.plot (iparam);
    }

    cpgsci (1);
    cpgbox(xaxis.c_str(),0,0,"bcnst",0,0);
    cpglab (xlabel.c_str(), info->get_name(iplot), "");

    ybottom += ybetween + yheight;

  }

  // restore the viewport
  cpgsvp (xmin, xmax, ymin, ymax);

}
catch (Error& error) {
  throw error += "Pulsar::CalibratorPlotter::plot(Calibrator::Info*)";
}
}
