//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/pgutil/Plot3D.h,v $
   $Revision: 1.2 $
   $Date: 2001/08/19 14:44:17 $
   $Author: straten $*/

#ifndef __CurvePlotter2D3_H
#define __CurvePlotter2D3_H

#include <vector>

#include "PlotVolume2D.h"
#include "DataManager.h"

namespace Plot2D {
  
  class CurvePlotter3 : public DataManager, public Plot2D::Volume
  {    
  public:
    
    CurvePlotter3() { init(); }
    virtual ~CurvePlotter3() {}
    
    // override virtual method of PlotVolume2D base class
    void painter();
    // need this in order to get world coordinates set by DataSets
    void rangeUpdate ();

    void drawPlot (DataSet* plot);

    // primitives
    void move (const Cartesian& pt);
    void draw (const Cartesian& pt);
    void plot (const Cartesian& pt, int symbol);
    void text (const Cartesian& pt, const char* text);

    void arrow (const Cartesian& from, const Cartesian& to);

    void poly (const vector<Cartesian>& pts);

    // set camera position - phi and theta in degrees
    void set_camera (double theta, double phi);

    void where (float& x, float& y, const Cartesian& pt);

  protected:
    Cartesian vx_axis;
    Cartesian vy_axis;

    void init(); 
  };
}

#endif
