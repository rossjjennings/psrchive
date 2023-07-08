/***************************************************************************
 *
 *   Copyright (C) 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ArrivalTime.h"
#include "Pulsar/ArchiveTemplates.h"
#include "Pulsar/AddNoise.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include "BoxMuller.h"
#include "RealTimer.h"

using namespace std;
using namespace Pulsar;

void test_arrival_time_uncertainty (unsigned ntest, const Archive* std, ArrivalTime* arrival)
{
  double total_shift_sq = 0;
  double total_error_sq = 0;

  AddNoise add_noise (0.01);

  double P = std->get_Integration(0)->get_folding_period();

  RealTimer timer;
  double update_period = 3.0;  // seconds
  double next_update = update_period;

  timer.start();

  for (unsigned i=0; i<ntest; i++)
  {
    Reference::To<Archive> copy = std->clone();
    foreach (copy, &add_noise);

    arrival->set_observation (copy);

    vector<Tempo::toa> toas;
    arrival->get_toas (toas);

    for (unsigned i = 0; i < toas.size(); i++)
    {
      double shift_turns = toas[i].get_phase_shift ();
      double shift_error_turns = toas[i].get_error()*1e-6 / P; 
      total_shift_sq += shift_turns * shift_turns;
      total_error_sq += shift_error_turns * shift_error_turns;
    }

    timer.stop();
    if (timer.get_total() > next_update)
    {
      cerr << "  iteration " << i << " / " << ntest << " efac=" << sqrt(total_shift_sq/total_error_sq) << endl;
      next_update += update_period;
    }
  }

  double vfac = total_shift_sq / total_error_sq;
  cout << "efac=" << sqrt(vfac) << " vfac=" << vfac << endl;
}
