//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/EstimatePlotter.h,v $
   $Revision: 1.4 $
   $Date: 2003/05/21 11:51:11 $
   $Author: straten $ */

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

  template<class Xt, class Yt>
  void add_plot (const vector<Xt>& xdata, const vector< Estimate<Yt> >& ydata);

  //! Plot the specified data set
  template<class T> void plot (const vector< Estimate<T> >& data);

  //! Plot the specified member of the current data set
  void plot (unsigned index);

  //! Plot the current data set
  void plot ();

 protected:

  //! Borders
  float x_border, y_border;

  //! add_plot X range
  float xrange_min, xrange_max;

  //! total X range
  float x_min, x_max;

  //! total Y range
  float y_min, y_max;

  //! range set
  bool range_set;

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

  if (!range_set) {
    y_min = data[0].val - errors[0];
    y_max = data[0].val + errors[0];
    x_min = xrange_min;
    x_max = xrange_max;
    range_set = true;
  }

  if (xrange_min < x_min)
    x_min = xrange_min;

  if (xrange_max < x_max)
    x_max = xrange_max;

  vector<float>& x = xval.back();
  vector<float>& y = yval.back();

  double xscale = double(xrange_max - xrange_min) / double(npt-1);

  for (ipt=1; ipt<npt; ipt++) {
    float yval = data[ipt].val - errors[ipt];
    if (yval < y_min)
      y_min = yval;
    yval = data[ipt].val + errors[ipt];
    if (yval > y_max)
      y_max = yval;

    x[ipt] = xrange_min + xscale * double(ipt);
    y[ipt] = data[ipt].val;
  }
}

template<class Xt, class Yt> 
void EstimatePlotter::add_plot (const vector<Xt>& xdata,
				const vector< Estimate<Yt> >& ydata)
{
  unsigned ipt = 0, npt = xdata.size();
  if (npt == 0)
    return;
  
  if (ydata.size() != npt)
    throw Error (InvalidParam, "EstimatePlotter::add_plot (Xt, Yt)",
		 "xdata.size=%d != ydata.size=%d", npt, ydata.size());

  xval.push_back ( vector<float>(npt) );
  yval.push_back ( vector<float>(npt) );
  yerr.push_back ( vector<float>(npt) );

  vector<float>& errors = yerr.back();
  for (ipt=0; ipt<npt; ipt++)
    errors[ipt] = sqrt (ydata[ipt].var);

  if (!range_set) {
    y_min = ydata[0].val - errors[0];
    y_max = ydata[0].val + errors[0];
    x_min = xdata[0];
    x_max = xdata[0];
    range_set = true;
  }

  vector<float>& x = xval.back();
  vector<float>& y = yval.back();

  for (ipt=1; ipt<npt; ipt++) {
    float yval = ydata[ipt].val - errors[ipt];
    if (yval < y_min)
      y_min = yval;
    yval = ydata[ipt].val + errors[ipt];
    if (yval > y_max)
      y_max = yval;

    float xval = xdata[ipt];
    if (xval < x_min)
      x_min = xval;
    if (xval > x_max)
      x_max = xval;

    x[ipt] = xval;
    y[ipt] = ydata[ipt].val;
  }
}

#endif
