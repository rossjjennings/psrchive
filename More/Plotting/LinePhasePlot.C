/***************************************************************************
*
*   Copyright (C) 2007 by David Smith
*   Licensed under the Academic Free License version 2.1
*
***************************************************************************/



#include "Pulsar/LinePhasePlot.h"
#include "Pulsar/Profile.h"



using namespace std;
using namespace Pulsar;



LinePhasePlot::LinePhasePlot()
{}



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
  return &data[row];
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
  // prepare a line for every subint using this formula
  
  // take the profile data
  // determine the maximum amplitude from all of the 
  
  /*
  unsigned first = 0;
  unsigned last = arch->get_nbin();
  
  vector<float> amps;
  float maxamp;
  float minamp;
  
  data.resize( last - first );
  
  int count = 0;
  for (unsigned i = first; i <= last; i++)
  {
    vector<float> tempamps = copy->get_Integration(i)->
	get_Profile(ipol,ichan)->get_weighted_amps();

    for (unsigned j = 0; j < nbin; j++)
    {
      amps[count] = tempamps[j];

      if (amps[count] > maxamp)	maxamp = amps[count];
      if (amps[count] < minamp)	minamp = amps[count];
      count++;
    }
  }
  float bias = maxamp / 3.0;
  
  count = 0;
  for (unsigned i = 0; i < nsub; i++)
  {
    for (unsigned j = 0; j < nbin; j++)
    {
      amps[count] += float(i)*bias;
      count++;
    }
    data[i]->set_amps( amps );
    cpgline(nbin,xaxis, amps + i*nbin );
  }
  */
}




