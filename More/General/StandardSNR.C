/***************************************************************************
 *
 *   Copyright (C) 2004-2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StandardSNR.h"

#include "Pulsar/ProfileStats.h"
using namespace std;

void Pulsar::StandardSNR::set_standard (const Profile* profile)
{
  fit.set_standard (profile);

  ProfileStats stats;
  stats.set_profile (profile);

  double on_sum = stats.get_total().get_value();
  double on_count = stats.get_onpulse()->get_weight_sum();

  standard_signal = on_sum / on_count;

  cerr << "std signal=" << standard_signal << endl;
}

float Pulsar::StandardSNR::get_snr (const Profile* profile)
{
  fit.set_Profile (profile);

  // factor of two determined empirically
  return 2 * fit.get_snr();

  cerr << "fit snr=" << fit.get_snr() << endl;

  //! Get the resulting scale factor
  double scale = fit.get_scale().get_value();

  //! Get the resulting Mean Squared Error (per fit DOF)
  double rms = sqrt( fit.get_mse() );

  cerr << "scale=" << scale << " rms=" << rms << endl;

  double snr = standard_signal * scale / rms;

  cerr << "snr ratio=" << fit.get_snr()/snr << endl;
}    


