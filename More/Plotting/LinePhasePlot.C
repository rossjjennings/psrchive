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
 * TODO     - Compact the lines together like the old pav
 **/

void LinePhasePlot::prepare (const Archive* arch )
{
  try
  {
    int ipol = 0;
    int ichan = 0;
    int isubint = -1;

    int nsub = arch->get_nsubint();
    int nbin = arch->get_nbin();

    int fsub = 0;
    int lsub = nsub - 1;

    float max_amp = FLT_MIN;
    float min_amp = FLT_MAX;

    data.empty();

    for( int s = fsub; s <= lsub; s ++ )
    {
        const float *orig_amps = arch->get_Integration(s)->get_Profile( ipol, ichan )->get_amps();
        vector<float> new_amps( nbin );

        for( int a = 0; a < nbin; a ++ )
        {
            new_amps[a] = orig_amps[a];
            if( new_amps[a] > max_amp )
                max_amp = new_amps[a];
            if( new_amps[a] < min_amp )
                min_amp = new_amps[a];
        }
	Reference::To<Profile> new_profile = new Profile();  
	new_profile->set_amps( new_amps );
	data.push_back( new_profile );	
    }

    // Set the scale and bias
    float range = max_amp - min_amp;
    get_frame()->get_y_scale()->set_minmax( min_amp - range * .4, max_amp + (range * 0.4)*nsub );
    float bias = (range) / 3;

    y_res = bias;
    y_scale = -1;

    set_line_colour( 7 );
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




