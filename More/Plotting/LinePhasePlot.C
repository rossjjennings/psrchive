/***************************************************************************
*
*   Copyright (C) 2007 by David Smith
*   Licensed under the Academic Free License version 2.1
*
***************************************************************************/



#include "Pulsar/LinePhasePlot.h"
#include "Pulsar/Profile.h"
#include <Pulsar/Archive.h>
#include <Pulsar/Integration.h>
#include <float.h>
#include <cpgplot.h>



using namespace std;
using namespace Pulsar;



LinePhasePlot::LinePhasePlot()
{
    style = "line";
    set_line_colour( 7 );
}



LinePhasePlot::~LinePhasePlot()
{}



/**
 * get_nrow
 *
 * DOES     - Return the number of rows, in this case the number of subints we wish to show
 * RECEIVES - The Archive to be displayed
 * RETURNS  - The number of rows
 * THROWS   - Nothing
 * TODO     - Nothing
 **/

unsigned LinePhasePlot::get_nrow (const Archive* arch)
{
    return data.size();
}




/**
 * get_Profile
 *
 * DOES     - Retreives a profile to be displayed
 * RECEIVES - The Archive and the row to retreive
 * RETURNS  - A Profile
 * THROWS   - Nothing
 * TODO     - Nothing
 **/

const Profile* LinePhasePlot::get_Profile (const Archive* arch, unsigned row)
{
  return data[row].ptr();
}



/**
 * prepare
 *
 * DOES     - Extract data from the archive and store it in data
 * RECEIVES - The Archive to extract from
 * RETURNS  - Nothing
 * THROWS   - Nothing
 * TODO     - Could make the spacing between integrations configurable
 **/

void LinePhasePlot::prepare (const Archive* arch )
{
  try
  {
    int nsub = arch->get_nsubint();
    int nbin = arch->get_nbin();

    int fsub = 0;
    int lsub = nsub - 1;
    
    float max_amp = FLT_MIN;
    float min_amp = FLT_MAX;

    data.clear();
    
    vector< vector< float > > all_data;
    
    all_data.resize( nsub );
    
    // create a massive data table with all the profiles we are looking at
    // while at it, determine the min and max amplitude from the whole table
    for( int s = fsub; s <= lsub; s ++ )
    {
      const float *src_amps = arch->get_Integration(s)->get_Profile( ipol, ichan )->get_amps();
      
      all_data[s-fsub].resize( nbin );
      for( int d = 0; d < nbin; d ++ )
      {
        all_data[s-fsub][d] = src_amps[d];
	if( src_amps[d] > max_amp )
	  max_amp = src_amps[d];
	if( src_amps[d] < min_amp )
	  min_amp = src_amps[d];
      }
    }
    
    float data_range = max_amp - min_amp;
    
    // scale all the data so that 1/3 of the data range will = 1
    // ie there is (1) between each profile and this represents 1/3 of the max range
    // put the resulting profiles into the data vector
    
    float dscale = 1 / data_range;
    for( int s = fsub; s <= lsub; s ++ )
    {
      for( int d = 0; d < nbin; d ++ )
      {
	all_data[s-fsub][d] *= dscale;
	all_data[s-fsub][d] += s;
      }
      Reference::To< Profile > new_profile = new Profile();
      new_profile->set_amps( all_data[s-fsub] );
      data.push_back( new_profile );
    }

    // set the range we want to look
    
    float target_min = fsub + (min_amp * dscale);
    float target_max = lsub + (max_amp * dscale);
    float target_range = target_max - target_min;
    
    // get the range specified by the user
    
    float set_min, set_max;
    get_frame()->get_y_scale()->get_range( set_min, set_max );
    
    // scale out target range according to the users set range
    
    float new_min = target_min + set_min * target_range;
    float new_max = target_max - (1-set_max) * target_range + (lsub);
    get_frame()->get_y_scale()->set_minmax( target_min, target_max );
    
    y_scale = -1;
    y_res = 0;
    
    // Don't draw an alternate axis, and label the y axis and set tick marks to 1 (ie we don't have 1/2 a subint)
    
    get_frame()->get_y_axis()->set_alternate( false );
    get_frame()->get_y_axis()->set_tick( 1.0 );
    get_frame()->get_y_axis()->set_label( "Sub Integration" );
  }
  catch( Error e )
  {
    cerr << "preparing LinePhasePlot failed with exception " << e << endl;
  }
}



/**
 * get_interface
 *
 * DOES     - Returns a text interface to this object
 * RECEIVES - Nothing
 * RETURNS  - text interface
 * THROWS   - Nothing
 * TODO     - Nothing
 **/

TextInterface::Parser *LinePhasePlot::get_interface()
{
  return new Interface( this );
}




