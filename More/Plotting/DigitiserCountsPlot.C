/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "Pulsar/DigitiserCountsPlot.h"
#include <Pulsar/DigitiserCounts.h>
#include <float.h>
#include <cpgplot.h>
#include <templates.h>



using namespace Pulsar;
using namespace std;



DigitiserCountsPlot::DigitiserCountsPlot()
{
  fsub = -1;
  lsub = -1;
  subint = -1;
}



TextInterface::Parser *DigitiserCountsPlot::get_interface()
{
  return new Interface( this );
}



void DigitiserCountsPlot::prepare( const Archive *data )
{
  Reference::To<Archive> ncdata = const_cast<Archive*>(data);
  if( ncdata )
  {
    Reference::To<DigitiserCounts> ext = ncdata->get<DigitiserCounts>();
    if( ext )
    {
      int num_rows = ext->rows.size();
      int npthist = ext->get_npthist();
      int ndigr = ext->get_ndigr();
      int data_length = npthist * ndigr;
      
      if( fsub == -1 && lsub == -1 )
      {
	if( subint == -1 )
	{
	  fsub = 0;
	  lsub = num_rows -1;
	}
	else
	{
	  fsub = subint;
	  lsub = subint;
	}
      }

      min_count = FLT_MAX;
      max_count = FLT_MIN;



      // Determine the first and last indices with non zero data in all channels/subints
      first_nz = npthist;
      last_nz = 0;
      for( int s = fsub; s <= lsub; s ++ )
      {
        for( int c = 0; c < ndigr; c ++ )
        {
          bool scanning_first = true;
          for( int v = 0; v < npthist; v ++ )
          {
            float next_value = ext->rows[s].data[c*npthist + v];
            if( next_value > 10 )
            {
              if( v < first_nz )
                first_nz = v;
              break;
            }
          }
          for( int v = npthist - 1; v >= 0; v -- )
          {
            float next_value = ext->rows[s].data[c*npthist + v];
            if( next_value > 10 )
            {
              if( v > last_nz )
                last_nz = v;
              break;
            }
          }
        }
      }

      // Find the min and max counts for all subints and channels
      for( int s = fsub ; s <= lsub; s ++ )
      {
	cyclic_minmax( ext->rows[s].data, first_nz, last_nz, min_count, max_count );
      }

      float range = max_count - min_count;
      y_jump = range * 0.75;
      // adjust the y_step to the nearest 10,000
      y_jump = int( y_jump / 10000 ) + 1;
      y_jump *= 10000;
      
      get_frame()->get_y_scale()->set_minmax( min_count , max_count + y_jump * (lsub - fsub) );
      get_frame()->get_y_scale()->set_buf_norm( 0.05 );
      get_frame()->get_x_scale()->set_minmax( 0, ext->get_ndigr() );
      get_frame()->hide_axes();
    }
  }
}



void DigitiserCountsPlot::draw( const Archive *data )
{
  Reference::To<Archive> clone = data->clone();

  Reference::To<DigitiserCounts> ext = clone->get<DigitiserCounts>();
  if( ext )
  {
    int ndigr = ext->get_ndigr();
    int npthist = ext->get_npthist();
    int data_length = ndigr * npthist;


    // the range of non zero indices
    int nz_range = (last_nz - first_nz) + 1;

    float xstep = 1.0 / nz_range;
    for( int s = fsub; s <= lsub; s ++ )
    {
      int old_col;
      cpgqci( &old_col );
      int next_col = old_col;

      for( int c = 0; c < ndigr; c ++ )
      {
        float xs[ nz_range ];
        float ys[ nz_range ];
        for( int j = 0; j < nz_range; j ++ )
        {
          xs[j] = j * xstep + c;
          ys[j] = ext->rows[s].data[(first_nz+j) + c * npthist ] + y_jump * s;
        }
        cpgsci( next_col++ );
        cpgbin( nz_range, xs, ys, 1 );
      }
      cpgsci( old_col );
    }

    float vp_y1, vp_y2;
    get_frame()->get_y_scale()->get_range_external( vp_y1, vp_y2 );

    // Save the current viewport, so that we can adjust relative to it and restore it later.
    float tx_min, tx_max, ty_min, ty_max;
    cpgqvp( 0, &tx_min, &tx_max, &ty_min, &ty_max );
    
    // For each channel set the viewport on that area of the graph and draw a box around it.
    float nx = tx_min;
    float xw = (tx_max-tx_min) * (1.0/ndigr);
    for( int c = 0; c < ndigr; c ++ )
    {
      // draw a box around the left and right viewports
      cpgsvp( nx, nx + xw, ty_min, ty_max );
      cpgswin( first_nz - 256, last_nz - 256, vp_y1, vp_y2 );
      if( nx == tx_min )
        cpgbox("bcnt", 0.0, 0, "bcnt", 0.0, 0);
      else
        cpgbox("bcnt", 0.0, 0, "bct", 0.0, 0 );
      string dig_label = string("Dig ") + tostring<int>(c);
      cpgmtxt( "T", -1, 0.01, 0, dig_label.c_str() );
      nx += xw;
    }

    cpgsvp( tx_min, tx_max, ty_min, ty_max );
  }
}



string DigitiserCountsPlot::get_xlabel( const Archive *data )
{
  return "Digitiser Value";
}


string DigitiserCountsPlot::get_ylabel( const Archive *data )
{
  return "Occurances";
}


