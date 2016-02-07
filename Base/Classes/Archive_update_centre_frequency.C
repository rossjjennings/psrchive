/***************************************************************************
 *
 *   Copyright (C) 2015 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 *   Moved from Archive_remove_chan by Stefan Oslowski
 *   This code is now used in DeleteInterpreter and remove_chan
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

using namespace std;

void Pulsar::Archive::update_centre_frequency()
{
  unsigned nchan = get_nchan();
  double new_center_freq=0.0;
  for (unsigned ichan=0; ichan<nchan; ichan++) 
    new_center_freq += get_Integration(0)->get_centre_frequency(ichan);
  new_center_freq /= (double)nchan;
  set_centre_frequency(new_center_freq);

  if (verbose == 3)
  {
    cerr << "Pulsar::Archive::update_centre_frequency updated centre frequency to " << new_center_freq << endl;
    cerr << "Pulsar::Archive::update_centre_frequency exit" << endl;
  }
}
