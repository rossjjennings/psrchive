#include "Pulsar/CalibratorPlotter.h"
#include "EstimatePlotter.h"

#include <cpgplot.h>

bool Pulsar::CalibratorPlotter::verbose = false;

Pulsar::CalibratorPlotter::CalibratorPlotter ()
{
}

Pulsar::CalibratorPlotter::~CalibratorPlotter ()
{
  // destructors must be defined in .C file so that the Reference::To
  // desctructor can delete forward declared objects
}


//! PGPLOT the calibrator model parameters as a function of frequency
void Pulsar::CalibratorPlotter::plot (const Calibrator* calib)
{
  init (calib);

  unsigned ipt = 0, npt = get_ndat ();

  if (npt == 0)
    return;

  float xmin, xmax, ymin, ymax;
  cpgqvp (0, &xmin, &xmax, &ymin, &ymax);

  float ybottom = ymin;
  float yrange = ymax - ymin;
  float yspace = 0.1 * yrange;
  float yheight = (yrange - yspace) / 3.0;

  cpgsci(1);
  cpgslw(1);
  cpgsch(1);

  // the plotting class
  EstimatePlotter plotter;

  // the data to be plotted
  vector< Estimate<float> > data (npt);

  // ////////////////////////////////////////////////////////////////////

  for (ipt=0; ipt<npt; ipt++)
    data[ipt] = get_gain( ipt );

  cpgsvp (xmin, xmax, ybottom, ybottom + yheight);

  plotter.plot (data);

  cpgbox("bcst",0,0,"bcnvst",0,0);
  cpgmtxt("L",2.5,.5,.5,"Gain");

  ybottom += 0.5*yspace + yheight;


  unsigned idim = 0, ndim = 0;

  // ////////////////////////////////////////////////////////////////////

  plotter.clear ();

  ndim = get_nboost ();
  for (idim=0; idim<ndim; idim++) {
    for (ipt=0; ipt<npt; ipt++)
      data[ipt] = get_boost (ipt, idim);

    plotter.add_plot (data);
  }

  cpgsvp(xmin, xmax, ybottom, ybottom + yheight);
  for (idim=0; idim<ndim; idim++) {
    cpgsci (idim+2);
    plotter.plot (idim);
  }

  cpgsci (1);
  cpgbox("bcst",0,0,"bcnvst",0,0);
  cpgmtxt("L",2.5,.5,.5,"Boost");

  ybottom += 0.5*yspace + yheight;

  // ////////////////////////////////////////////////////////////////////

  plotter.clear ();

  ndim = get_nboost ();
  for (idim=0; idim<ndim; idim++) {
    for (ipt=0; ipt<npt; ipt++)
      data[ipt] = get_rotation (ipt, idim);

    plotter.add_plot (data);
  }

  cpgsvp(xmin, xmax, ybottom, ybottom + yheight);
  for (idim=0; idim<ndim; idim++) {
    cpgsci (idim+2);
    plotter.plot (idim);
  }

  cpgsci (1);
  cpgbox("bcst",0,0,"bcnvst",0,0);
  cpgmtxt("L",2.5,.5,.5,"Phase");

  // restore the viewport
  cpgsvp (xmin, xmax, ymin, ymax);

}







