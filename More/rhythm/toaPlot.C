#include "toaPlot.h"

#include "Error.h"

#include "minmax.h"

toaPlot::toaPlot (QWidget *parent, const char *name )
  : QPgplot (parent, name)
{
  x.resize(0);
  y.resize(0);
  npts = 0;
  
  xmin = ymin = 0.0;
  xmax = ymax = 1.0;
  
  xq = yq = None;
  
  mode = 0;
  clicks = 0;

  requestEvent(mode);
}

// define pure virtual method of QtPgplot base class
void toaPlot::plotter ()
{
  cpgsci (1);
  cpgsls (1);
  cpgslw (1);
  cpgsvp  (0.1,0.9,0.1,0.9);
  
  cpgswin (xmin, xmax, ymin, ymax);
  cpgbox ("bcnst",0.0,0,"bcnst",0.0,0);
  
  string xlab, ylab;

  switch (xq) {
  case None:
    xlab = " ";
    break;
  case ResidualMicro:
    xlab = "Residual Microseconds";
    break;
  case ResidualMilliTurns:
    xlab = "Residual Milliturns";
    break;
  case TOA_MJD:
    xlab = "Arrival Time (Days Since MJD 50000)";
    break;
  }

  switch (yq) {
  case None:
    ylab = " ";
    break;
  case ResidualMicro:
    ylab = "Residual Microseconds";
    break;
  case ResidualMilliTurns:
    ylab = "Residual Milliturns";
    break;
  case TOA_MJD:
    ylab = "Arrival Time (MJD)";
    break;
  }

  cpglab (xlab.c_str(), ylab.c_str(), "");
  
  cpgsci (2);
  cpgsls (2);
  cpgslw (4);
  cpgpt (npts, &(x.front()), &(y.front()), 1);

}

void toaPlot::handleEvent (float x, float y, char ch)
{ 
  cerr << "toaPlot::handleEvent x=" << x << " y=" << y << " ch=" << ch << endl;
  
  if (ch == '~') {
    clicks = 0;
    clearScreen();
    drawPlot();
  }
  else {    
    switch (mode) {
    case 4:
      if (clicks == 0) {
	xmin = x;
	clicks++;
	break;
      }
      if (clicks == 1) {
	xmax = x;
	clicks = 0;
	clearScreen();
	drawPlot();
	break;
      }
    case 3:
      if (clicks == 0) {
	ymin = y;
	clicks++;
	break;
      }
      if (clicks == 1) {
	ymax = y;
	clicks = 0;
	clearScreen();
	drawPlot();
	break;
      }
    }
  }
  
  requestEvent(mode);
}

void toaPlot::ider ()
{
  mode = 0;
  handleEvent(0,0,'~');
}

void toaPlot::xzoomer ()
{
  mode = 4;
  handleEvent(0,0,'~');
}

void toaPlot::yzoomer ()
{
  mode = 3;
  handleEvent(0,0,'~');
}

void toaPlot::setPoints(AxisQuantity _xq, vector<double> xpts, 
			AxisQuantity _yq, vector<double> ypts)
{
  if (xpts.size() != ypts.size())
    throw Error(InvalidParam, "toaPlot::setPoints array sizes not equal");

  npts = xpts.size();

  if (npts == 0) return;

  xq = _xq;
  yq = _yq;
  
  x.resize(0);
  y.resize(0);
  
  for (unsigned i = 0; i < npts; i++) {
    x.push_back(float(xpts[i]));
    y.push_back(float(ypts[i]));
  }

  findminmax(&(x.front()), &(x.back()), xmin, xmax);
  findminmax(&(y.front()), &(y.back()), ymin, ymax);
  
  xmin = xmin - fabs(xmin/20.0);
  xmax = xmax + fabs(xmax/20.0);
  ymin = ymin - fabs(ymin/20.0);
  ymax = ymax + fabs(ymax/20.0);

  clearScreen();
  drawPlot();
}










