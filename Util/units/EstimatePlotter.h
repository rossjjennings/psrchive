//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/EstimatePlotter.h,v $
   $Revision: 1.2 $
   $Date: 2003/05/09 00:27:15 $
   $Author: hknight $ */

#ifndef __EstimatePlotter_h
#define __EstimatePlotter_h

#include "Estimate.h"
#include <vector>

#include "psr_cpp.h"

class EstimatePlotter {

 public:

  //! Default constructor
  EstimatePlotter ();

  //! Clear all data
  void clear ();

  //! Set the border used when setting the world coordinates of the viewport
  void set_border (float fraction_x, float fraction_y);

  //! Set the range of x values
  void set_xrange (float x_min, float x_max);

  //! Add a vector of Estimates to the current data set
  template<class T> void add_plot (const vector< Estimate<T> >& data);

  //! Plot the specified data set
  template<class T> void plot (const vector< Estimate<T> >& data);

  //! Plot the specified member of the current data set
  void plot (unsigned index);

  //! Plot the current data set
  void plot ();

 protected:

  //! Borders
  float x_border, y_border;

  //! X range
  float x_min, x_max;

  //! Y range
  float y_min, y_max;

  //! Y range set
  bool y_range;

  vector< vector<float> > xval;
  vector< vector<float> > yval;
  vector< vector<float> > yerr;
  
};

template<class T> 
void EstimatePlotter::plot (const vector< Estimate<T> >& data)
{
  clear ();
  add_plot (data);
  plot (0);
}

template<class T> 
void EstimatePlotter::add_plot (const vector< Estimate<T> >& data)
{
  unsigned ipt = 0, npt = data.size();
  if (npt == 0)
    return;
  
  xval.push_back ( vector<float>(npt) );
  yval.push_back ( vector<float>(npt) );
  yerr.push_back ( vector<float>(npt) );

  vector<float>& errors = yerr.back();
  for (ipt=0; ipt<npt; ipt++)
    errors[ipt] = sqrt (data[ipt].var);

  if (!y_range) {
    y_min = data[0].val - errors[0];
    y_max = data[0].val + errors[0];
    y_range = true;
  }

  vector<float>& x = xval.back();
  vector<float>& y = yval.back();

  double xscale = double(x_max - x_min) / double(npt-1);

  for (ipt=1; ipt<npt; ipt++) {
    float yval = data[ipt].val - errors[ipt];
    if (yval < y_min)
      y_min = yval;
    yval = data[ipt].val + errors[ipt];
    if (yval > y_max)
      y_max = yval;

    x[ipt] = xscale * double(ipt);
    y[ipt] = data[ipt].val;
  }
}

#endif
