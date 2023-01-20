/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ArchiveExpert.h"
#include "Pulsar/Integration.h"

using namespace std;

void Pulsar::Archive::remove_chan (unsigned first, unsigned last)
{
  unsigned cur_nsub = get_nsubint();
  unsigned cur_nchan = get_nchan();
  double bw = get_bandwidth();
  double chan_bw = bw / (double)cur_nchan;

  if (first>cur_nchan-1 || last>cur_nchan-1)
    throw Error(InvalidRange, "Pulsar::Archive::remove_chan"
        "channel range invalid");

  if (first>last) {
    unsigned tmp;
    tmp = first;
    first = last;
    last = tmp;
  }

  unsigned nchan_to_remove = last - first + 1;
  unsigned new_nchan = cur_nchan - nchan_to_remove;

  if (verbose == 3)
    cerr << "Archive::remove_chan first=" << first
      << " last=" << last << " old nchan=" << cur_nchan
      << " new nchan=" << new_nchan << endl;

  for (unsigned isub=0; isub<cur_nsub; isub++) 
    get_Integration(isub) -> remove (first, last);

  if (verbose == 3)
    cerr << "Pulsar::Archive::remove_chan calling book-keeping functions" 
      << endl;

  set_nchan (new_nchan);
  set_bandwidth(bw - (double)nchan_to_remove * chan_bw);
  // Do we always want to reset center freq?
  if (new_nchan>0) {
    update_centre_frequency();
  }

  if (verbose == 3)
    cerr << "Pulsar::Archive::remove_chan exit" << endl;
}
