#include "EstimatePlotter.h"
#include "Error.h"

#include <cpgplot.h>

EstimatePlotter::EstimatePlotter ()
{
  x_border = y_border = 0.05;
  x_min = 0.0;
  x_max = 1.0;
  y_min = 0.0;
  y_max = 1.0;

  y_range = false;
}

void EstimatePlotter::plot ()
{
  for (unsigned index=0; index<xval.size(); index++)
    plot (index);
}

void EstimatePlotter::set_xrange (float xmin, float xmax)
{
  x_min = xmin;
  x_max = xmax;
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

  for (unsigned ipt=0; ipt<npt; ipt++)
    cpgerr1 (6, xval[index][ipt], yval[index][ipt], yerr[index][ipt], 1.0);
}

void EstimatePlotter::clear ()
{
  xval.resize(0);
  yval.resize(0);
  yerr.resize(0);
  y_range = false;
}
