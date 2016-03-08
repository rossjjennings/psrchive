/***************************************************************************
 *
 *   Copyright (C) 2016 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PatchFrequency.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/BasicIntegration.h"

#include "Pulsar/ArchiveExpert.h"
#include "Pulsar/IntegrationExpert.h"

#include <iostream>
using namespace std;

//! Default constructor
Pulsar::PatchFrequency::PatchFrequency ()
{
  freq_tol = 0.010; // 10 kHz
}

//! Add new zero-weighted profiles with the specified frequencies 
//  into the Integration
void Pulsar::PatchFrequency::add_channels (Integration *into, 
    vector<double> frequencies) const
{
  // Method: build a new integration with the necessary extra channels
  // for each archive.  Integration::insert() will take care of
  // adding them and sorting in the correct order.
  Pulsar::BasicIntegration *newint = new BasicIntegration;
  newint->expert()->resize(into->get_npol(), 
      frequencies.size(), into->get_nbin());
  newint->zero(); // May not be needed?
  newint->uniform_weight(0.0);
  for (unsigned i=0; i<frequencies.size(); i++) 
    newint->set_centre_frequency(i, frequencies[i]);
  into->expert()->insert(newint);
}

void Pulsar::PatchFrequency::add_channels (Archive *into,
    vector<double> frequencies) const
{
  for (unsigned isub=0; isub<into->get_nsubint(); isub++)
    add_channels(into->get_Integration(isub), frequencies);

  // Update Archive top-level attributes:
  const unsigned nchanorig = into->get_nchan();
  const unsigned nchannew = into->get_nchan() + frequencies.size();
  into->expert()->set_nchan(nchannew);
  into->set_bandwidth(into->get_bandwidth()*nchannew/nchanorig);
  double fcent=0;
  for (unsigned i=0; i<nchannew; i++) 
    fcent += into->get_Integration(0)->get_centre_frequency(i);
  fcent /= (double)nchannew;
  into->set_centre_frequency(fcent);
}

//! Add to A whatever is missing with respect to B, and vice versa
void Pulsar::PatchFrequency::operate (Archive* A, Archive* B) try
{

  // Figure out the set of channels in each archive that already
  // appear in the other.
  vector<bool> A_chans_in_B (A->get_nchan(), false);
  vector<bool> B_chans_in_A (B->get_nchan(), false);

  // For now, just match using the frequencies from the first subint
  Pulsar::Integration* subA = A->get_Integration(0);
  Pulsar::Integration* subB = B->get_Integration(0);
  for (unsigned ia=0; ia<A->get_nchan(); ia++) 
  {
    for (unsigned ib=0; ib<B->get_nchan(); ib++) 
    {
      if (fabs(subA->get_centre_frequency(ia) - 
            subB->get_centre_frequency(ib)) < freq_tol) 
      {
        A_chans_in_B[ia] = true;
        B_chans_in_A[ib] = true;
      }
    }
  }

  // The number of existing channels that match, and that need to be added
  // to each array:
  unsigned naddA = count(B_chans_in_A.begin(), B_chans_in_A.end(), false);
  unsigned naddB = count(A_chans_in_B.begin(), A_chans_in_B.end(), false);

  // If there is nothing to add to either archive, we are done
  if (naddA==0 && naddB==0) { return; }

  // List of new frequencies to add to each
  vector<double> faddA, faddB;
  for (unsigned ia=0; ia<A->get_nchan(); ia++) 
    if (!A_chans_in_B[ia]) 
      faddB.push_back(subA->get_centre_frequency(ia)); 
  for (unsigned ib=0; ib<B->get_nchan(); ib++) 
    if (!B_chans_in_A[ib]) 
      faddA.push_back(subB->get_centre_frequency(ib)); 

  // Add the necessary channels
  if (naddA) add_channels(A, faddA);
  if (naddB) add_channels(B, faddB);

}
catch (Error& error)
{
  throw error += "Pulsar::PatchFrequency::operate";
}

