#include "EstimatePlotter.h"
#include "Error.h"

#include <cpgplot.h>

EstimatePlotter::EstimatePlotter ()
{
  x_border = y_border = 0.05;
  xrange_min = x_min = y_min = 0.0;
  xrange_max = x_max = y_max = 1.0;

  minimum_error = maximum_error = -1.0;

  graph_marker = -1;

  range_set = false;
}

void EstimatePlotter::plot ()
{
  for (unsigned index=0; index<xval.size(); index++)
    plot (index);
}

void EstimatePlotter::set_xrange (float xmin, float xmax)
{
  xrange_min = xmin;
  xrange_max = xmax;
}

//! Set the minimum value of error to plot
void EstimatePlotter::set_minimum_error (float error)
{
  minimum_error = error;
}

//! Set the maximum value of error to plot
void EstimatePlotter::set_maximum_error (float error)
{
  maximum_error = error;
}

/*! See <a href="http://www.astro.caltech.edu/~tjp/pgplot/chapter4.html>
Section 4.4</a> of the PGPLOT  Graphics Subroutine Library User's Manual. */
void EstimatePlotter::set_graph_marker (int symbol)
{
  graph_marker = symbol;
}


void EstimatePlotter::plot (unsigned index)
{
  if (index >= xval.size())
    throw Error (InvalidRange, "EstimatePlotter::plot",
		 "iplot=%d >= nplot=%d", index, xval.size());

  float xbuf = x_border * (x_max-x_min);
  float ybuf = y_border * (y_max-y_min);

  cpgswin (x_min-xbuf, x_max+xbuf, y_min-ybuf, y_max+ybuf);

  unsigned npt = xval[index].size();

  for (unsigned ipt=0; ipt<npt; ipt++) {

    if (minimum_error >= 0.0 && yerr[index][ipt] <= minimum_error)
      continue;

    if (maximum_error >= 0.0 && yerr[index][ipt] >= maximum_error)
      continue;

    cpgerr1 (6, xval[index][ipt], yval[index][ipt], yerr[index][ipt], 1.0);
    cpgpt1 (xval[index][ipt], yval[index][ipt], graph_marker);

  }

}


void EstimatePlotter::clear ()
{
  xval.resize(0);
  yval.resize(0);
  yerr.resize(0);
  range_set = false;
}
