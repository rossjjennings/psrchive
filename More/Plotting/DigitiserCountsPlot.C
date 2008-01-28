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



/**
 * Constructor
 *
 *  DOES     - Inititialization
 *  RECEIVES - Nothing
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

DigitiserCountsPlot::DigitiserCountsPlot()
{
  subint = -1;
  srange.first = -1;
  srange.second = -1;
  valid_data = false;
}



/**
 * get_interface
 *
 *  DOES     - Creates a text interfaces to this object
 *  RECEIVES - Nothing
 *  RETURNS  - a DigitiserCountsPlot::Interface pointer
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

TextInterface::Parser *DigitiserCountsPlot::get_interface()
{
  return new Interface( this );
}



/**
 * CheckCounts
 *
 *  DOES     - Do a few basic checks to see if that Archive given has a valid DigitiserCounts extension.
 *             if we can't find a problem with it, set valid_data to true.
 *  RECEIVES - The extension to examine
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

void DigitiserCountsPlot::CheckCounts( const Archive *const_data )
{
  Archive *data = const_cast<Archive*>( const_data );

  Reference::To<DigitiserCounts> ext = data->get<DigitiserCounts>();

  if( !ext )
  {
    if( verbose > 1 )
      cerr << "Attempted to plot DigitiserCounts on an archive without a DigitiserCounts extension" << endl;
    return;
  }

  int npthist = ext->get_npthist();
  int ndigr = ext->get_ndigr();
  int nlev = ext->get_nlev();

  if( npthist == 0 || nlev == 0 || ndigr == 0 )
  {
    if( verbose > 1 )
      cerr << "Attempted to plot DigitiserCounts on an archive with parameter (npthist,ndigr,nlev)" << endl;
    return;
  }
  
  valid_data = true;
}



/**
 * prepare
 *
 *  DOES     - Determines what values to use for the x and y axes
 *  RECEIVES - The archive to examine
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

void DigitiserCountsPlot::prepare( const Archive *data )
{
  CheckCounts( data );

  if( !valid_data )
    return;

  Reference::To<Archive> ncdata = const_cast<Archive*>(data);
  if( !ncdata )
    return;

  Reference::To<DigitiserCounts> ext = ncdata->get<DigitiserCounts>();

  if( !ext )
    return;


  int num_rows = ext->rows.size();
  int npthist = ext->get_npthist();
  int ndigr = ext->get_ndigr();

  // Make sure srange.first,srange.second have the subint range we want to display
  if( srange.first == -1 && srange.second == -1 )
  {
    if( subint == -1 )
    {
      srange.first = 0;
      srange.second = num_rows -1;
    }
    else
    {
      srange.first = subint;
      srange.second = subint;
    }
  }

  // Should optimize this, at present just create a separate array for data thats scaled properly
  adjusted_data.resize( num_rows );
  for( int s = 0; s < num_rows; s ++ )
  {
    adjusted_data[s].resize( ndigr * npthist );
    for( int d = 0; d < ndigr * npthist; d ++ )
    {
      adjusted_data[s][d] = float(ext->rows[s].data[d]) * ext->rows[s].data_scl + ext->rows[s].data_offs;
    }
  }

  // Determine the first and last indices with non zero data in all channels/subints
  // Some files have data values close to zero but negligible, treat anything under
  // 10 as effectively zero.
  first_nz = npthist;
  last_nz = 0;
  for( int s = srange.first; s <= srange.second; s ++ )
  {
    for( int c = 0; c < ndigr; c ++ )
    {
      bool scanning_first = true;
      for( int v = 0; v < npthist; v ++ )
      {
        //float next_value = ext->rows[s].data[c*npthist + v];
        float next_value = adjusted_data[s][c*npthist + v];
        if( next_value > 10 )
        {
          if( v < first_nz )
            first_nz = v;
          break;
        }
      }
      for( int v = npthist - 1; v >= 0; v -- )
      {
        // float next_value = ext->rows[s].data[c*npthist + v];
        float next_value = adjusted_data[s][c*npthist + v];
        if( next_value > 10 )
        {
          if( v > last_nz )
            last_nz = v;
          break;
        }
      }
    }
  }
  // sanity check, if first_nz >= last_nz, then we have all zeroes, just plot the whole thing
  if( first_nz >= last_nz )
  {
    first_nz = 0;
    last_nz = npthist;
  }

  // Find the min and max counts for all subints and channels
  min_count = FLT_MAX;
  max_count = FLT_MIN;
  for( int s = srange.first ; s <= srange.second; s ++ )
  {
    //cyclic_minmax( ext->rows[s].data, first_nz, last_nz, min_count, max_count );
    cyclic_minmax( adjusted_data[s], first_nz, last_nz, min_count, max_count );
  }

  // y_range is how far up the y axis each subint goes
  float y_range = max_count - min_count;

  // y_jump is how far to increase y between subints, adjust to nearest 10,000
  y_jump = y_range * 0.75;
  y_jump = int( y_jump / 10000 ) + 1;
  y_jump *= 10000;

  // set the ranges for the axes, we hide the axes becase we need to draw a box for
  // each digitiser channel later.
  // TODO find out why we need to hard code the buffer here
  get_frame()->get_y_scale()->set_minmax( min_count , max_count + y_jump * (srange.second - srange.first) );
  get_frame()->get_y_scale()->set_buf_norm( 0.05 );
  get_frame()->get_x_scale()->set_minmax( 0, ext->get_ndigr() );
  get_frame()->hide_axes();
}



/**
 * draw
 *
 *  DOES     - Renders the histagrams
 *  RECEIVES - The archive to render
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

void DigitiserCountsPlot::draw( const Archive *data )
{
  Reference::To<Archive> clone = data->clone();

  if( !valid_data )
    return;

  Reference::To<DigitiserCounts> ext = clone->get<DigitiserCounts>();
  if( !ext )
    return;

  int ndigr = ext->get_ndigr();
  int npthist = ext->get_npthist();

  // the range of non zero indices
  int nz_range = (last_nz - first_nz) + 1;

  // for each subint
  //   save the old color
  //   for each digitiser channel
  //     for each index in the non zero range
  //       calculate the next x coord
  //       calculate the next y coord
  //     draw a binned histogram of x and y
  //   restore the old color
  float xstep = 1.0 / nz_range;
  for( int s = srange.first; s <= srange.second; s ++ )
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
        // ys[j] = ext->rows[s].data[(first_nz+j) + c * npthist ] + y_jump * (s-srange.first);
        ys[j] = adjusted_data[s][(first_nz+j) + c * npthist] + y_jump * (s-srange.first);
      }
      cpgsci( next_col++ );
      cpgbin( nz_range, xs, ys, 1 );
    }
    cpgsci( old_col );
  }

  // get the viewport range including buffer
  float vp_y1, vp_y2;
  get_frame()->get_y_scale()->get_range_external( vp_y1, vp_y2 );

  // Save the current viewport, so that we can adjust relative to it and restore it later.
  float tx_min, tx_max, ty_min, ty_max;
  cpgqvp( 0, &tx_min, &tx_max, &ty_min, &ty_max );

  // For each digitiser channel
  //   set the viewport to cover that channel
  //   set the window to have x ranging from -256 to 255
  //   draw a box around the viewport
  float nx = tx_min;
  float xw = (tx_max-tx_min) * (1.0/ndigr);
  for( int c = 0; c < ndigr; c ++ )
  {
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

  // Restore the original viewport
  cpgsvp( tx_min, tx_max, ty_min, ty_max );
}



/**
 * get_xlabel
 *
 *  DOES     - Returns label for x axis
 *  RECEIVES - The archive
 *  RETURNS  - "Digitiser Value"
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

string DigitiserCountsPlot::get_xlabel( const Archive *data )
{
  return "Digitiser Value";
}



/**
 * get_ylabel
 *
 *  DOES     - Returns the label for y axis
 *  RECEIVES - The archive
 *  RETURNS  - "Occurances"
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

string DigitiserCountsPlot::get_ylabel( const Archive *data )
{
  return "Occurances";
}


