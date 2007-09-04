/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ProfileVectorPlotter.h"
#include "Pulsar/PlotFrame.h"
#include "Pulsar/Profile.h"

#include <cpgplot.h>
#include <vector>

using namespace std;


double mod( double dividend, double quotient )
{
  return dividend - double(int(dividend/quotient));
}




Pulsar::ProfileVectorPlotter::ProfileVectorPlotter ()
{
  plot_histogram = false;
}

void Pulsar::ProfileVectorPlotter::minmax (PlotFrame* frame) const
{
  if (!profiles.size())
    throw Error (InvalidState, "Pulsar::ProfileVectorPlotter::minmax",
		 "no Profiles");

  unsigned nbin = profiles[0]->get_nbin();

  unsigned i_min, i_max;

  frame->get_x_scale()->get_range (nbin, i_min, i_max);

  float min = profiles[0]->min(i_min, i_max);
  float max = profiles[0]->max(i_min, i_max);

  for (unsigned iprof=1; iprof < profiles.size(); iprof++) {
    float pmin = profiles[iprof]->min(i_min, i_max);
    float pmax = profiles[iprof]->max(i_min, i_max);
    if (pmin < min)
      min = pmin;
    if (pmax > max)
      max = pmax;
  }

  frame->get_y_scale()->set_minmax (min, max);
}

void Pulsar::ProfileVectorPlotter::draw ( float sx, float ex )
{
  for (unsigned iprof=0; iprof < profiles.size(); iprof++) {

    if (x.size() != profiles[iprof]->get_nbin()) {
      x.resize(profiles[iprof]->get_nbin());
      for (unsigned i=0; i<x.size(); i++)
	x[i] = i;
    }
	       
    if (plot_sci.size() == profiles.size())
      cpgsci (plot_sci[iprof]);
    else
      cpgsci (iprof+1);

    if (plot_sls.size() == profiles.size())
      cpgsls (plot_sls[iprof]);
    else
      cpgsls (iprof+1);

    draw (profiles[iprof], sx, ex );
  }
}

//! draw the profile in the current viewport and window
void Pulsar::ProfileVectorPlotter::draw (const Profile* profile, float sx, float ex ) const
{
  vector< float > data;
  profile->get_amps( data );
  int data_pts = data.size();
  
  int total_pts = int( ( ex - sx ) * data_pts );
  
  float xs[total_pts];
  float ys[total_pts];
  
  // Fill the xs array with the x coordinates
  float dx = ( ex - sx ) / float(total_pts);
  float next_x = sx;
  for( int i = 0; i < total_pts; i ++ )
  {
    xs[i] = next_x;
    next_x += dx;
  }

  // calculate the starting point index
  float r = mod( sx, 1 );
  int pt_index;
  if( r >= 0 )
    pt_index = int( data_pts * r );
  else
    pt_index = data_pts + int( data_pts * r );

    // fill the ypts array with the points from our data set repeatedly
  for( int i = 0; i < total_pts; i ++ )
  {
    ys[i] = data[pt_index++];
    if( pt_index > data_pts )
      pt_index = 0;
  }

  // cpgline( total_pts, xs, ys );
  
  if (plot_histogram)
    cpgbin (total_pts, xs, ys, true);
  else
    cpgline (total_pts, xs, ys );
}
