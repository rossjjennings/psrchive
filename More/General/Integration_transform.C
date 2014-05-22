/***************************************************************************
 *
 *   Copyright (C) 2014 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/Profile.h"
#include "Error.h"

namespace Pulsar {

  /* Transform a full-poln profile using the PolnProfile class */
  static void transform4(Signal::Basis basis, Signal::State state, 
      Profile *prof0, Profile *prof1, Profile *prof2, Profile *prof3,
      const Jones<float>& response)
  {
    PolnProfile poln (basis, state, 
                      prof0, prof1,
                      prof2, prof3);

    poln.transform (response);
  }

  /* Transform a two-poln profile if the Jones matrix is diagonal */
  static void transform2(Profile *prof0, Profile *prof1, 
      const Jones<float>& response)
  {
    if (!response.is_diagonal())
      throw Error (InvalidState, "Pulsar::Integration::transform (transform2)",
          "2-pol transform requires diagonal Jones matrix");

    // Apply the scaling to each poln
    prof0->scale((response.j00*conj(response.j00)).real());
    prof1->scale((response.j11*conj(response.j11)).real());
  }

}

void Pulsar::Integration::transform (const Jones<float>& response)
{
  if (get_npol() != 4 && get_npol() != 2)
    throw Error (InvalidState, "Pulsar::Integration::transform",
		 "incomplete polarization information");

  Signal::Basis basis = get_basis();
  Signal::State state = get_state();

  for (unsigned ichan=0; ichan < get_nchan(); ichan++) try {

    if (get_npol() == 4) 
      transform4 (basis, state, 
                  profiles[0][ichan], profiles[1][ichan],
                  profiles[2][ichan], profiles[3][ichan],
                  response);

    else if (get_npol() == 2) 
      transform2 (profiles[0][ichan], profiles[1][ichan], response);

  }
  catch (Error& error) {
    if (verbose)
      cerr << "Pulsar::Integration::transform error ichan=" << ichan
           << error << endl;
    set_weight (ichan, 0);
  }
}
 
void Pulsar::Integration::transform (const vector< Jones<float> >& response)
{
  if (get_npol() != 4 && get_npol() != 2)
    throw Error (InvalidState, "Pulsar::Integration::transform",
		 "incomplete polarization information");

  if (response.size() != get_nchan())
    throw Error (InvalidState, "Pulsar::Integration::transform",
		 "response size=%d != nchan=%d", response.size(), get_nchan());

  if (verbose)
    cerr << "Pulsar::Integration::transform vector<Jones<float>>" << endl;

  Signal::Basis basis = get_basis();
  Signal::State state = get_state();

  for (unsigned ichan=0; ichan < get_nchan(); ichan++) try {

    if (get_npol() == 4)
      transform4 (basis, state, 
                  profiles[0][ichan], profiles[1][ichan],
                  profiles[2][ichan], profiles[3][ichan],
                  response[ichan]);

    else if (get_npol() == 2) 
      transform2 (profiles[0][ichan], profiles[1][ichan], response[ichan]);
    
  }
  catch (Error& error) {
    if (verbose)
      cerr << "Pulsar::Integration::transform error ichan=" << ichan
           << error << endl;
    set_weight (ichan, 0);
  }

}
 

