#include "Pulsar/BaselineWindow.h"

// #define _DEBUG 1

#ifdef _DEBUG
#include <iostream>
using namespace std;
#endif

// defined in Profile.C
void nbinify (int& istart, int& iend, int nbin);

//! Default constructor
Pulsar::BaselineWindow::BaselineWindow ()
{
  duty_cycle = 0.15;
  bin_start = bin_end = 0;
  range_specified = false;
  find_max = false;
}

void Pulsar::BaselineWindow::set_Profile (const Profile* profile)
{
}

//! Retrieve the PhaseWeight
void Pulsar::BaselineWindow::get_weight (PhaseWeight& weight)
{
}

//! Set the duty cycle
void Pulsar::BaselineWindow::set_duty_cycle (float _duty_cycle)
{
  if (_duty_cycle <= 0 || _duty_cycle >= 1)
    throw Error (InvalidParam, "Pulsar::BaselineWindow::set_duty_cycle",
		 "invalid duty_cycle=%f", _duty_cycle);

  duty_cycle = _duty_cycle;
}

float Pulsar::BaselineWindow::get_duty_cycle () const
{
  return duty_cycle;
}

//! Set to find the minimum mean
void Pulsar::BaselineWindow::set_find_minimum ()
{
  find_max = false;
}
  
//! Set to find the maximum mean
void Pulsar::BaselineWindow::set_find_maximum ()
{
  find_max = true;
}

//! Set the start and end bins of the search
void Pulsar::BaselineWindow::set_range (int start, int end)
{
  bin_start = start;
  bin_end = end;
  range_specified = true;
}


//! Return the phase at which minimum or maximum mean is found
float Pulsar::BaselineWindow::find_phase (unsigned nbin, float* amps)
{
  unsigned boxwidth = unsigned (.5 * duty_cycle * nbin);
  if (boxwidth >= nbin/2 || boxwidth == 0)
    throw Error (InvalidParam, "Pulsar::BaselineWindow::find_phase",
		 "invalid duty_cycle=%f -> boxwidth=%d (nbin= %d)\n",
		 duty_cycle, boxwidth*2+1, nbin);

#ifdef _DEBUG
  cerr << "Pulsar::BaselineWindow::find_phase duty_cycle=" << duty_cycle
       << " boxwidth=" << boxwidth*2+1 << endl;
#endif
  
  unsigned left = nbin-boxwidth;
  unsigned right = nbin+boxwidth+1;
  unsigned found_bin = 0;
  
  if (range_specified) {
    nbinify (bin_start, bin_end, nbin);
    left = bin_start;
    found_bin = bin_start + boxwidth;
    right = bin_start + 2*boxwidth+1;
  }
  
#ifdef _DEBUG
  cerr << "Pulsar::BaselineWindow::find_phase init left=" << left 
       << " found=" << found_bin << " right=" << right << endl;
#endif
  
  double sum = 0.0;
  for (unsigned ibin=left; ibin<right; ibin++)
    sum += amps[ibin % nbin];
  
  unsigned stop = 2*nbin-boxwidth;
  if (range_specified)
    stop = bin_end - (2*boxwidth+1);
  
  double found_val = sum;
  
#ifdef _DEBUG
  cerr << "Pulsar::BaselineWindow::find_phase search stop=" << stop 
       << " found_val=" << found_val << endl;
#endif

  while (left < stop)  {

#ifdef _DEBUG
    cerr << "add=" << right%nbin << " sub=" << left%nbin << endl;
#endif

    sum += amps[right%nbin] - amps[left%nbin];

    left++;
    right++;

    if ( (find_max && sum > found_val) || (!find_max && sum < found_val) ) {

      found_val = sum;
      found_bin = left + boxwidth;

#ifdef _DEBUG
      cerr << "new=" << found_val << " bin=" << found_bin << endl;
#endif

    }

  }

  float phase = float(found_bin%nbin) / float(nbin);

#ifdef _DEBUG
  cerr << "phase=" << phase << endl;
#endif

  return phase;

}
 
