/***************************************************************************
 *
 *   Copyright (C) 2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Correlate.h"

#include <iostream>
using namespace std;

int main (int argc, char** argv) try
{
  unsigned nbin = 70;
  Pulsar::Profile std (nbin);
  Pulsar::Profile obs (nbin);

  // std = standard = template = a one-sided triangle with a peak in bin 10
  std.zero();
  auto std_amps = std.get_amps();
  std_amps[10] = 2.0;
  std_amps[11] = 1.0;

  // obs = observation = input = a one-sided triangle with a peak in bin 20
  obs.zero();
  auto obs_amps = obs.get_amps();
  obs_amps[20] = 2.0;
  obs_amps[21] = 1.0;

  // correlate the input with the template in place
  Pulsar::Correlate correlator;
  correlator.set_normalize(false);
  correlator.set_operand(&std);
  correlator.transform(&obs);

  /*
    expected result: the template (peak at 10) maximally overlaps with the observation (peak at 20)
    when the offset is 10 samples/bins; therefore, the peak in the cross-correlation is expected in bin 10.
  */
  std::vector<float> expect (nbin, 0.0);
  expect[9] = 2.0;   // 1 times 2
  expect[10] = 5.0;  // 2 times 2 plus 1 times 1
  expect[11] = 2.0;  // 2 times 1

  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
    if (obs_amps[ibin] != expect[ibin])
    {
      cerr << ibin << " " << obs_amps[ibin] << " != expected value" << expect[ibin] << endl;
      return -1;
    }
  }

  cerr << "test passed" << endl;
  return 0;
}
catch (Error & error)
{
  cerr << "test_Function_load: error" << error << endl;
  return -1;
}
