#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>

#include <qhbox.h>
#include <qpushbutton.h>

#include <cpgplot.h>

#include "qpgplot.h"

class toaPlot: public QPgplot {
  
  Q_OBJECT

 public:
  
  enum AxisQuantity { None, ResidualMicro, ResidualMilliTurns, 
		      TOA_MJD };
  
  toaPlot ( QWidget *parent=0, const char *name=0 );
  
  // define pure virtual methods of QtPgplot base class
  void plotter ();
  void handleEvent (float x, float y, char ch);
  
  void setPoints (AxisQuantity, vector<double>, 
		  AxisQuantity, vector<double>);
  
 public slots:

  void xzoomer ();
  void yzoomer ();
  void ider ();

 protected:

  // Information about data display

  vector<float> x;
  vector<float> y;
  unsigned npts;
  
  float xmin, xmax, ymin, ymax;
  AxisQuantity xq, yq;
  
  int mode;

  // Interactivity buffers
  unsigned clicks;
};



