//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/EstimatePlotter.h,v $
   $Revision: 1.7 $
   $Date: 2003/10/16 13:38:50 $
   $Author: straten $ */

#ifndef __EstimatePlotter_h
#define __EstimatePlotter_h

#include "Estimate.h"
#include "Error.h"

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

  //! Set the minimum value of error to plot
  void set_minimum_error (float error);

  //! Set the maximum value of error to plot
  void set_maximum_error (float error);

  //! Set the PGPLOT standard graph marker
  void set_graph_marker (int symbol);

  //! Set flag to plot data in separate viewports
  void separate_viewports (bool scaled = true, bool vertical = true);

  //! Set the viewport to plot the specified member of the current data set
  void set_viewport (unsigned index);

  //! Set the world coordinates of the viewport with some buffer space
  void set_world (float x1, float x2, float y1, float y2);

  //! Set the viewport to that when separate_viewports was called
  void restore_viewport ();

  //! Add a vector of Estimates to the current data set
  template<class T> void add_plot (const vector< Estimate<T> >& data);

  template<class Xt, class Yt>
  void add_plot (const vector<Xt>& xdata, const vector< Estimate<Yt> >& ydata);

  //! Plot the specified data set
  template<class T> void plot (const vector< Estimate<T> >& data);

  //! Plot the specified member of the current data set
  void plot (unsigned index);

  //! Plot the current data set in one window
  void plot ();

  void minmax (bool& xrange_set, float& xmin, float& xmax,
	       bool& yrange_set, float& ymin, float& ymax,
	       const vector<float>& x,
	       const vector<float>& y,
	       const vector<float>& yerr);

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
  bool xrange_set;
  bool yrange_set;

  //! The minimum error in plot
  float minimum_error;

  //! The maximum error in plot
  float maximum_error;

  //! PGPLOT Standard Graph Marker
  int graph_marker;

  vector< vector<float> > xval;
  vector< vector<float> > yval;
  vector< vector<float> > yerr;
  
 private:

  vector<float> data_xmin;
  vector<float> data_xmax;
  vector<float> data_ymin;
  vector<float> data_ymax;

  float vp_x1, vp_x2, vp_y1, vp_y2;

  bool viewports_set;
  bool viewports_vertical;
  bool viewports_scaled;

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

  vector<float>& x = xval.back();
  vector<float>& y = yval.back();
  vector<float>& ye = yerr.back();

  double xscale = double(xrange_max - xrange_min) / double(npt-1);

  for (ipt=0; ipt<npt; ipt++) {
    ye[ipt] = sqrt (data[ipt].var);
    x[ipt] = xrange_min + xscale * double(ipt);
    y[ipt] = data[ipt].val;
  }

  minmax (xrange_set, x_min, x_max, yrange_set, y_min, y_max, x, y, ye);
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

  vector<float>& x = xval.back();
  vector<float>& y = yval.back();
  vector<float>& ye = yerr.back();

  for (ipt=0; ipt<npt; ipt++) {
    ye[ipt] = sqrt (ydata[ipt].var);
    x[ipt] = xdata[ipt];
    y[ipt] = ydata[ipt].val;
  }

  minmax (xrange_set, x_min, x_max, yrange_set, y_min, y_max, x, y, ye);
}

#endif
