#include "toaPlot.h"
#include "Error.h"
#include "minmax.h"

wrapper::wrapper () {  
  x = 0.0;
  y = 0.0;
  e = 0.0; 
  ci = 7;
  dot = 0;
  id = 0;
}

toaPlot::toaPlot (QWidget *parent, const char *name )
  : QPgplot (parent, name)
{
  data.resize(0);

  xmin = ymin = 0.0;
  xmax = ymax = 1.0;
  
  xq = yq = None;
  
  mode = 0;
  task = 1;
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
  case BinaryPhase:
    xlab = "Binary Phase";
    break;
  case ObsFreq:
    xlab = "Observing Frequency";
    break;
  case DayOfYear:
    xlab = "Day of Year";
    break;
  case ErrorMicro:
    xlab = "Timing Error (Microseconds)";
    break;
  case SignalToNoise:
    xlab = "Signal / Noise Ratio";
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
  case BinaryPhase:
    ylab = "Binary Phase";
    break;
  case ObsFreq:
    ylab = "Observing Frequency";
    break;
  case DayOfYear:
    ylab = "Day of Year";
    break;
  case ErrorMicro:
    ylab = "Timing Error (Microseconds)";
    break;
  case SignalToNoise:
    ylab = "Signal / Noise Ratio";
    break;
  }

  cpgsci (3);
  cpglab (xlab.c_str(), ylab.c_str(), "");
  cpgsci (1);

  for (unsigned i = 0; i < data.size(); i++) {

    cpgsci  (data[i].ci);
    cpgpt1  (float(data[i].x), float(data[i].y), data[i].dot);

    if (data[i].e != 0.0)
      cpgerr1 (6, float(data[i].x), float(data[i].y), float(data[i].e), 1.0);

  }

  cpgsci(1);
}

void toaPlot::handleEvent (float x, float y, char ch)
{ 
  //cerr << "toaPlot::handleEvent x=" << x 
  //     << " y=" << y << " ch=" << ch << endl;
  
  if (ch == '~') {
    clicks = 0;
    clearScreen();
    drawPlot();
  }
  else {
    switch (mode) {
    case 0:
      if (task == 1) {
	//Do Nothing
      }
      else if (task == 2) {

	if (data.empty()) 
	  break;

	int tempint = data[0].id;
	float distance = 0.0;

	float min = sqrt(pow(x-float(data[0].x),2.0) + pow(y-float(data[0].y),2.0));

	for (unsigned i = 1; i < data.size(); i++) {
	  distance = sqrt(pow(x-float(data[i].x),2.0) + pow(y-float(data[i].y),2.0));
	  if (distance < min) {
	    min = distance;
	    tempint = data[i].id;
	  }
	}
	emit selected(tempint);
	emit ineednewdata();
	handleEvent(0,0,'~');
      }
      break;
    case 4:
      if (task == 1) {
	if (clicks == 0) {
	  x1 = x;
	  clicks++;
	  break;
	}
	if (clicks == 1) {
	  x2 = x;
	  clicks = 0;
	  if (x1 > x2) {
	    xmin = x2;
	    xmax = x1;
	  }
	  else {
	    xmin = x1;
	    xmax = x2;
	  }
	  clearScreen();
	  drawPlot();
	  break;
	}
      }
      else if (task == 2) {
	if (clicks == 0) {
	  x1 = x;
	  clicks++;
	  break;
	}
	if (clicks == 1) {
	  x2 = x;
	  clicks = 0;
	  if (x1 > x2) {
	    float handy1 = x1;
	    float handy2 = x2;
	    x2 = handy1;
	    x1 = handy2;
	  } 
	  for (unsigned i = 0; i < data.size(); i++) {
	    if ((data[i].x > x1) && (data[i].x < x2))
	      emit selected(data[i].id); 
	  }
	  emit ineednewdata();
	  break;
	}
      }
    case 3:
      if (task == 1) {
	if (clicks == 0) {
	  y1 = y;
	  clicks++;
	  break;
	}
	if (clicks == 1) {
	  y2 = y;
	  clicks = 0;
	  if (y1 > y2) {
	    ymin = y2;
	    ymax = y1;
	  }
	  else {
	    ymin = y1;
	    ymax = y2;
	  }
	  clearScreen();
	  drawPlot();
	  break;
	}
      }
      else if (task == 2) {
	if (clicks == 0) {
	  y1 = y;
	  clicks++;
	  break;
	}
	if (clicks == 1) {
	  y2 = y;
	  clicks = 0;
	  if (y1 > y2) {
	    float handy1 = y1;
	    float handy2 = y2;
	    y2 = handy1;
	    y1 = handy2;
	  } 
	  for (unsigned i = 0; i < data.size(); i++) {
	    if ((data[i].y > y1) && (data[i].y < y2))
	      emit selected(data[i].id);
	  }
	  emit ineednewdata();
	  break;
	}
      }
    case 2:
      if (task == 1) {
	if (clicks == 0) {
	  x1 = x;
	  y1 = y;
	  clicks++;
	  break;
	}
	if (clicks == 1) {
	  x2 = x;
	  y2 = y;
	  clicks = 0;
	  if (y1 > y2) {
	    ymin = y2;
	    ymax = y1;
	  }
	  else {
	    ymin = y1;
	    ymax = y2;
	  }
	  if (x1 > x2) {
	    xmin = x2;
	    xmax = x1;
	  }
	  else {
	    xmin = x1;
	    xmax = x2;
	  }
	  clearScreen();
	  drawPlot();
	  mode = 0;
	  handleEvent(0,0,'~');
	  break;
	}
      }
      else if (task == 2) {
	if (clicks == 0) {
	  x1 = x;
	  y1 = y;
	  clicks++;
	  break;
	}
	if (clicks == 1) {
	  x2 = x;
	  y2 = y;
	  clicks = 0;
	  if (x1 > x2) {
	    float handy1 = x1;
	    float handy2 = x2;
	    x2 = handy1;
	    x1 = handy2;
	  } 
	  if (y1 > y2) {
	    float handy1 = y1;
	    float handy2 = y2;
	    y2 = handy1;
	    y1 = handy2;
	  } 
	  for (unsigned i = 0; i < data.size(); i++) {
	    if ((data[i].y > y1) && (data[i].y < y2) && (data[i].x > x1) && (data[i].x < x2))
	      emit selected(data[i].id); 
	  }
	  emit ineednewdata();
	  mode = 0;
	  handleEvent(0,0,'~');
	  break;
	}
      }
    }
  }
  
  requestEvent(mode,x,y);
}

void toaPlot::ptselector ()
{
  mode = 0;
  task = 2;
  handleEvent(0,0,'~');
}

void toaPlot::xzoomer ()
{
  mode = 4;
  task = 1;
  handleEvent(0,0,'~');
}

void toaPlot::yzoomer ()
{
  mode = 3;
  task = 1;
  handleEvent(0,0,'~');
}

void toaPlot::boxzoomer ()
{
  mode = 2;
  task = 1;
}

void toaPlot::xselector ()
{
  mode = 4;
  task = 2;
  handleEvent(0,0,'~');
}

void toaPlot::yselector ()
{
  mode = 3;
  task = 2;
  handleEvent(0,0,'~');
}

void toaPlot::boxselector ()
{
  mode = 2;
  task = 2;
}

void toaPlot::setPoints(AxisQuantity _xq, AxisQuantity _yq, vector<wrapper> _data)
{
  if (_data.empty()) {
    clearScreen();
    return;
  }
  
  xq = _xq;
  yq = _yq;

  data = _data;

  clearScreen();
  drawPlot();
}

void toaPlot::autoscale ()
{
  if (data.empty()) return;
  
  vector<float> xpts;
  vector<float> ypts;
  
  for (unsigned i = 0; i < data.size(); i++) {
    xpts.push_back(float(data[i].x));
    ypts.push_back(float(data[i].y));
  }
  
  findminmax(&(xpts.front()), &(xpts.back()), xmin, xmax);
  findminmax(&(ypts.front()), &(ypts.back()), ymin, ymax);
  
  clearScreen();
  drawPlot();
}








