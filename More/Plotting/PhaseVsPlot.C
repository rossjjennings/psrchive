/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseVsPlot.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"
#include "templates.h"
#include "Pulsar/PhaseScale.h"

#include <cpgplot.h>

#include <algorithm>

#include <float.h>

using namespace std;

Pulsar::PhaseVsPlot::PhaseVsPlot ()
{
  colour_map.set_name( pgplot::ColourMap::Heat );

  // default is to invert the axis tick marks
  get_frame()->get_x_axis()->set_opt ("BCINTS");

  get_frame()->get_y_axis()->set_opt ("BINTS");
  get_frame()->get_y_axis()->set_alternate (true);

  get_frame()->get_label_above()->set_offset (1.0);

  // ensure that no labels are printed inside the frame
  get_frame()->get_label_below()->set_all (PlotLabel::unset);

  style = "image";
  line_colour = -1;

  y_res = -1;
  y_scale = -1;
}

TextInterface::Parser* Pulsar::PhaseVsPlot::get_interface ()
{
  return new Interface (this);
}

void Pulsar::PhaseVsPlot::set_style (const string& s)
{
  if (s != "image" && s != "line")
    throw Error (InvalidParam, "Pulsar::PhaseVsPlot::set_style",
                 "invalid style '" + s + "'");
  style = s;
}

//! Derived classes must draw in the current viewport
void Pulsar::PhaseVsPlot::draw (const Archive* data)
{
  colour_map.apply ();

  float x_min = 0.0;
  float x_max = 1.0;
  get_scale()->get_range( x_min, x_max );
  
  float y_min = 0.0;
  float y_max = 1.0;
  get_frame()->get_y_scale()->PlotScale::get_minmax (y_min, y_max);

  // Fill the image data
  unsigned nbin = data->get_nbin();
  unsigned nrow = get_nrow (data);

  bool cyclic = true;
  unsigned min_bin, max_bin;
  get_frame()->get_x_scale()->get_indeces (nbin, min_bin, max_bin, cyclic);

  unsigned min_row, max_row;
  get_frame()->get_y_scale()->get_indeces (nrow, min_row, max_row);

  float min = FLT_MAX;
  float max = FLT_MIN;

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
  if( y_res == -1 )
    y_res = (y_max-y_min)/nrow;



  if (style == "image")
  {

    get_z_scale()->set_minmax (min, max);
    get_z_scale()->get_range (min, max);

    pair<float,float> range = get_scale()->get_range_norm();

    int range_start = int(floor(range.first));
    int range_end = int(ceil(range.second));

    for( int range = range_start; range < range_end; range ++ )
    {
      float xoff = float(range) * get_scale()->get_scale(data);

      // X = TR(0) + TR(1)*I + TR(2)*J
      // Y = TR(3) + TR(4)*I + TR(5)*J

      float trf[6] = { xoff + x_min - 0.5*x_res, x_res, 0.0,
                       y_min - 0.5*y_res,        0.0, y_res };

      cpgimag(&plotarray[0], nbin, nrow, 1, nbin, 1, nrow, min, max, trf);
    }

  }
  else if (style == "line")
  {
    if( line_colour != -1 )
      cpgsci( line_colour );

    get_z_scale()->set_minmax (0, max);
    get_z_scale()->get_range (min, max);

    vector<float> xaxis;
    get_scale()->get_ordinates (data, xaxis);

    vector<float> xaxis_adjusted;
    xaxis_adjusted.resize( nbin );

    if( y_scale != -1 )
      y_scale = y_res/max;
    else
      y_scale = 1;

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

    float first_x, last_x, x_step;
    Pulsar::PhaseScale::Units units = get_scale()->get_units();
    if( units == Pulsar::PhaseScale::Turns )
    {
      first_x = floor(x_min) - 1;
      last_x = ceil(x_max) + 1;
      x_step = 1;
    }
    else if( units == Pulsar::PhaseScale::Degrees )
    {
      first_x = int(x_min / 360) * 360;
      last_x = int(x_max / 360) * 360;
      x_step = 360;
    }
    else
      throw Error( InvalidState, "Pulsar::PhaseVsPlot::draw",
		   "unhandled PhaseScale::Units" );

    for( int xoff = first_x; xoff < last_x; xoff += x_step )
    {
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

    cpgswin (x_min, x_max, min, max);

    cpgbox (" ", 0.0, 0, "CMIST", 0.0, 0);
    cpgmtxt("R", 2.6, 0.5, 0.5, "Index");
  }
  else
  {
    cpgbox( " ", 0.0, 0, "C", 0.0, 0 );
  }

}
