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
 * TODO     - There should be no need to pass the Archive all the time, why get the row numbers from
 *            one archive and draw another?
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
 * TODO     - There should be no need to pass the Archive all the time, why get the row numbers from
 *            one archive and draw another?
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
 * TODO     - Define this more clearly and implement.
 **/

void LinePhasePlot::prepare (const Archive* arch )
{
    // set the min amp to 1000
    // set the max amp to -1000

    // for each subint in the range we are examining
    //   get the profile for the subint
    //   get the amps from the profile
    //   for each amplitude
    //     if this amplitude is greater than the previous max
    //       set max amplitude to this amp
    //     if this amplitude is less than the preivous min
    //       set the min amp to this amp
    //   add a profile to the data array with these amps

    // calculate the bias as 1/3 of the max amp

    // for each profile in the data array
    //   get the amps for the profile
    //   for each amp
    //     multiply the amp by the bias times the profile index
    //   set the amps for the profile to the new amps
  
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
	
	cerr << "( " << min_amp << ", " << max_amp << " )" << endl;
    }
    
    // TODO: fix up this hack for setting the scale.
    get_frame()->get_y_scale()->set_minmax( min_amp, max_amp );

    float bias = (max_amp - min_amp) / 3;

    for( int p = 0; p < data.size(); p ++ )
    {
        const float *next_amps = data[p]->get_amps();
        vector<float> adj_amps(nbin);
        for( int a = 0; a < nbin; a ++ )
        {
            adj_amps[a] = next_amps[a] + p * bias;
        }
        data[p]->set_amps( adj_amps );
    }

  }
  catch( Error e )
  {
    cerr << "preparing LinePhasePlot failed with exception " << e << endl;
  }


  set_line_colour( 7 );





    /*
     
     
     
     
     
      // prepare a line for every subint using this formula
     
      // take the profile data
      // determine the maximum amplitude from all of the
     
      unsigned int ipol = 0;
      unsigned int ichan = 0;
      unsigned int nbin = arch->get_nbin();
      unsigned int nsub = arch->get_nsubint();
     
      unsigned int fsub = 0;
      unsigned int lsub = nsub - 1;
     
      Reference::To<Archive> copy = arch->clone();
     
      vector<float> amps;
      float maxamp;
      float minamp;
     
      amps.resize( nbin * nsub );
     
      data.resize( nsub );
     
    // for each subint
    //   get a profile of the subint
    //   for each bin in the profile
    //     store the amplitude in the amps array
    //       if the new amp is greater than previous max
    //         store this as the max amp
    //       if the new amp is less than the previous min
    //         store this as the min amp
     
      int count = 0;
      for (unsigned i = fsub; i <= lsub; i++)
        {
          vector<float> tempamps = copy->get_Integration(i)->
                                   get_Profile(ipol,ichan)->get_weighted_amps();
     
          for (unsigned j = 0; j < nbin; j++)
            {
              amps[count] = tempamps[j];
     
              if (amps[count] > maxamp)
                maxamp = amps[count];
              if (amps[count] < minamp)
                minamp = amps[count];
              count++;
            }
        }
     
    // calculate the bias as 1/3 of the max amplitude
     
      float bias = maxamp / 3.0;
     
    // for each subint
    //   for each bin
    //     add a third of the maximum amplitude * the subint number to the amp
    //   set the newly created 
     
      count = 0;
      for (unsigned i = 0; i < nsub; i++)
        {
          for (unsigned j = 0; j < nbin; j++)
            {
              amps[count] += float(i)*bias;
              count++;
            }
          data[i].set_amps( &amps[i*nbin] );
        }*/
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

TextInterface::Class *LinePhasePlot::get_interface()
{
  return new Interface( this );
}




