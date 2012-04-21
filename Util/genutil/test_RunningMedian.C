/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "RunningMedian.h"
#include "BoxMuller.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <time.h>

using namespace std;

int main () 
{
  BoxMuller noise (time(NULL));

  unsigned nth_min = 2;
  unsigned nth_max = 100;

  unsigned nsamp = 10000;
  unsigned errors = 0;

  for (unsigned nth=nth_min; nth < nth_max; nth++)
  {
    vector<double> samples (nsamp);
    generate (samples.begin(), samples.end(), noise);

    // for the old method
    vector<double> window (2*nth+1);

    // new method
    RunningMedian<double> rmedian (nth);
    for (unsigned ipt=0; ipt < window.size(); ipt++)
      rmedian.insert( samples[ipt] );
    
    for (unsigned isamp = 0; isamp < nsamp-window.size(); isamp++)
    {
      // new method
      double new_median = rmedian.get_median();
      rmedian.erase( samples[isamp] );
      rmedian.insert( samples[isamp+window.size()] );

      // the old brute force method
      for (unsigned ipt=0; ipt < window.size(); ipt++)
	window[ipt] = samples[ipt+isamp];

      std::nth_element (window.begin(), window.begin()+nth, window.end());
      double old_median = window[nth];

      if (new_median != old_median)
      {
	cerr << "nth=" << nth << " isamp=" << isamp 
	     << " new=" << new_median << " old=" << old_median << endl;
	errors ++;
      }
    }
  }

  return 0;
}
