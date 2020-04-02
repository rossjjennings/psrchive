/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseVsPlot.h"
#include "Pulsar/PhaseScale.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"
#include "Pulsar/RemoveBaseline.h"

#include "templates.h"

#include <cpgplot.h>

#include <algorithm>

#include <float.h>
#include "strutil.h"

using namespace std;

Pulsar::PhaseVsPlot::PhaseVsPlot ()
{
  stringstream ss;
  ss << pgplot::ColourMap::default_colour_map;
  pgplot::ColourMap::Name _name;
  ss >> _name;
  colour_map.set_name( _name );

  // default is to invert the axis tick marks
  get_frame()->get_x_axis()->set_opt ("BCINTS");

  get_frame()->get_y_axis()->set_opt ("BINTS");
  get_frame()->get_y_axis()->set_alternate (true);

  get_frame()->get_label_above()->set_offset (1.0);

  // ensure that no labels are printed inside the frame
  get_frame()->get_label_below()->set_all (PlotLabel::unset);

  style = "image";
  line_colour = -1;
  
  crop_value = 1.0f;
}

TextInterface::Parser* Pulsar::PhaseVsPlot::get_interface ()
{
  return new Interface (this);
}

void Pulsar::PhaseVsPlot::preprocess (Archive* archive)
{
  RemoveBaseline::Each each;
  each (archive);
}

void Pulsar::PhaseVsPlot::set_style (const string& s)
{
  if (s != "image" && s != "line")
    throw Error (InvalidParam, "Pulsar::PhaseVsPlot::set_style",
                 "invalid style '" + s + "'");
  style = s;
}

void Pulsar::PhaseVsPlot::set_rows( const std::pair<unsigned,unsigned>& r )
{
  get_frame()->get_y_scale()->set_index_range (r);
}

std::pair<unsigned,unsigned> Pulsar::PhaseVsPlot::get_rows() const
{
  return const_cast<PlotFrame*>(get_frame())->get_y_scale()->get_index_range();
}

//! Derived classes must draw in the current viewport
void Pulsar::PhaseVsPlot::draw (const Archive* data)
{
  colour_map.apply ();

  float x_min, x_max;
  get_frame()->get_x_scale()->PlotScale::get_minmax (x_min, x_max);

  float y_min, y_max;
  get_frame()->get_y_scale()->PlotScale::get_minmax (y_min, y_max);

  if (verbose) {
    cerr << "Pulsar::PhaseVsPlot::draw y_min=" << y_min << " y_max=" << y_max
      << endl;
  }

  // Fill the image data
  unsigned nbin = data->get_nbin();

  bool cyclic = true;
  unsigned min_bin, max_bin;
  get_frame()->get_x_scale()->get_indeces (nbin, min_bin, max_bin, cyclic);

  unsigned nrow = get_frame()->get_y_scale()->get_num_indeces ();

  unsigned min_row, max_row;
  get_frame()->get_y_scale()->get_indeces (nrow, min_row, max_row);

  float min = FLT_MAX;
  float max = -FLT_MAX;

  vector<float> plotarray (nbin * nrow);
  for (unsigned irow = 0; irow < nrow; irow++)
  {
    vector<float> amps  = get_Profile (data, irow) -> get_weighted_amps();
    for (unsigned ibin=0; ibin<nbin; ibin++)
      plotarray[irow*nbin + ibin] = amps[ibin];

    if (irow < min_row || irow >= max_row)
      continue;

    cyclic_minmax (amps, min_bin, max_bin, min, max, true );
  }

  float x_res = (x_max-x_min)/nbin;
  float y_res = (y_max-y_min)/nrow;
  
  /* Added by DS, we crop the data range with crop_value being a
     percentage of max. However, if min is negative and greater in 
     magnitude then the new max, then we set min to be -max in order
     to get a zoom that is summetric around zero. */

  if( crop_value != 1.0f )
  {
    max *= crop_value;
    if( min < 0 && max + min < 0 )
      min = -max;
  }

  pair<float,float> range = get_scale()->get_range_norm();
  
  int range_start = int(floor(range.first));
  int range_end = int(ceil(range.second));

  if (style == "image")
  {
    get_z_scale()->set_minmax (min, max);
    get_z_scale()->get_range (min, max);

    for( int range = range_start; range < range_end; range ++ )
    {
      float xoff = float(range) * get_scale()->get_scale(data);

      // X = TR(0) + TR(1)*I + TR(2)*J
      // Y = TR(3) + TR(4)*I + TR(5)*J

      // cerr << " xoff=" << xoff << " x_min=" << x_min << " x_res=" << x_res
      //      << " y_min=" << y_min << " y_res=" << y_res << endl;

      float trf[6] = { xoff + x_min - 0.5f*x_res, x_res, 0.0f,
                       y_min - 0.5f*y_res,        0.0f, y_res };

      if (get_frame()->get_transpose())
	for (unsigned i=0; i<3; i++)
	  std::swap (trf[i], trf[i+3]);

      cpgimag(&plotarray[0], nbin, nrow, 1, nbin, 1, nrow, min, max, trf);
    }

  }
  else if (style == "line")
  {
    if( line_colour != -1 )
      cpgsci( line_colour );

    get_z_scale()->set_minmax (min, max);
    get_z_scale()->get_range (min, max);

    vector<float> xaxis;
    get_scale()->get_ordinates (data, xaxis);

    vector<float> xaxis_adjusted;
    xaxis_adjusted.resize( nbin );

    float y_scale = y_res/max;

    vector<bool> all_zeroes;
    all_zeroes.resize( max_row );

    for ( unsigned irow = min_row; irow < max_row; irow ++ )
    {
      all_zeroes[irow] = true;
      for( unsigned ibin=0; ibin < nbin; ibin ++ )
      {
        float amp = plotarray[irow*nbin + ibin];
        if (amp != 0.0)
          all_zeroes[irow] = false;
        float new_amp = amp * y_scale + y_res * irow;
        plotarray[irow*nbin + ibin] = new_amp;
      }
    }

    for( int range = range_start; range < range_end; range ++ )
    {
      float xoff = float(range) * get_scale()->get_scale(data);

      for( unsigned b = 0; b < nbin; b ++ )
        xaxis_adjusted[b] = xaxis[b] + xoff;

      for ( unsigned irow = min_row; irow < max_row; irow++ )
      {
        if (!all_zeroes[irow])
          cpgline (nbin, &xaxis_adjusted[0], &plotarray[irow*nbin]);

	// fill in the gap between iterations
	float pxs[2], pys[2];
	pxs[0] = xaxis_adjusted[nbin-1];
	pxs[1] = pxs[0] + ( 1.0 / float(nbin) );
	pys[0] = plotarray[irow*nbin + nbin - 1];
	pys[1] = plotarray[irow*nbin];
	cpgline( 2, pxs, pys );
      }
    }

    if( line_colour != -1 )
      cpgsci( 1 );
  }

  if (get_frame()->get_y_axis()->get_alternate())
  {

    float min, max;
    get_frame()->get_y_scale()->PlotScale::get_range (min, max);

    float length = y_max - y_min;

    min = (min - y_min) / length;
    max = (max - y_min) / length;

    min *= nrow;
    max *= nrow;

    if (get_frame()->get_transpose())
    {
      cpgswin (min, max, x_min, x_max);
      cpgbox ("CMIST", 0.0, 0, " ", 0.0, 0);
      cpgmtxt("T", 2.6, 0.5, 0.5, "Index");
    }
    else
    {
      cpgswin (x_min, x_max, min, max);
      cpgbox (" ", 0.0, 0, "CMIST", 0.0, 0);
      cpgmtxt("R", 2.6, 0.5, 0.5, "Index");
    }
  }

}
