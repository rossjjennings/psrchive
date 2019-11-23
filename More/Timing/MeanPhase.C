/***************************************************************************
 *
 *   Copyright (C) 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/MeanPhase.h"
#include "Pulsar/PhaseWeight.h"

using namespace std;

Estimate<double> Pulsar::MeanPhase::get_shift () const
{
  Reference::To<PhaseWeight> baseline = observation->baseline();
  float mean = baseline->get_avg();
  // float rms = baseline->get_rms();

  const float* amps = observation->get_amps();
  unsigned nbin = observation->get_nbin();

  double cosine = 0;
  double sine = 0;
  double weight = 0;

  double bin2rad = 2.0 * M_PI / nbin;

  for (unsigned ibin=0; ibin<nbin; ibin++)
  {
    double wt = amps[ibin] - mean;

    cosine += wt * cos( ibin * bin2rad );
    sine += wt * sin( ibin * bin2rad );
    weight += wt;

#if _DEBUG
   
    double theta = ibin * bin2rad;
    double phase = atan2 (sine, cosine) / (2.0*M_PI);

    cerr << ibin << " " << weight << " " << theta << " " << phase
	 << " " << cosine/weight
	 << " " << sine/weight << endl;

#endif
  }

  double phase = atan2 (sine, cosine) / (2.0*M_PI);
  return phase;
}
