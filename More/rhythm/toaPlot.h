#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>

#include <qhbox.h>
#include <qpushbutton.h>

#include <cpgplot.h>

#include "qpgplot.h"

class wrapper {

 public:

  wrapper();

  double x;
  double y;
  double e;

  int ci;
  int dot;

  int id;
};

class toaPlot: public QPgplot {
  
  Q_OBJECT

 public:
  
  enum AxisQuantity { None, ResidualMicro, ResidualMilliTurns, 
		      TOA_MJD, BinaryPhase, ObsFreq, DayOfYear};
  
  toaPlot ( QWidget *parent=0, const char *name=0 );
  
  // define pure virtual methods of QtPgplot base class
  void plotter ();
  void handleEvent (float x, float y, char ch);
  
  void setPoints (AxisQuantity, AxisQuantity, vector<wrapper>);
  
 public slots:

  void xzoomer ();
  void yzoomer ();
  void ptselector ();
  void xselector ();
  void yselector ();
  void boxzoomer ();
  void boxselector ();

  void autoscale ();

 signals:

  void selected (int);

 protected:

  // Information about data display

  vector<wrapper> data;
  
  float xmin, xmax, ymin, ymax;
  AxisQuantity xq, yq;
  
  int mode;
  int task;

  // Interactivity buffers
  unsigned clicks;
  
  int   tempint;
  float distance;
  float min;

  float x1;
  float x2;
  float y1;
  float y2;
};




