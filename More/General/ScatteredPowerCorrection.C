/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ScatteredPowerCorrection.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Physical.h"

#include "JenetAnderson98.h"

#define ESTIMATE_SNR 0

//! Default constructor
Pulsar::ScatteredPowerCorrection::ScatteredPowerCorrection ()
{
}

void Pulsar::ScatteredPowerCorrection::transform (Integration* data)
{
  unsigned nchan = data->get_nchan();
  unsigned nbin  = data->get_nbin();
  unsigned npol  = data->get_npol();

  double duration = data->get_duration();

  bool convert_state = false;

  if (npol == 4)  {
    if (data->get_state() != Signal::Coherence)
      throw Error (InvalidParam, "Pulsar::ScatteredPowerCorrection::transform",
		   "data with npol==4 must have state=Signal::Coherence");
    npol = 2;
  }

  // check that scattered power is not significantly dispersed
  double chan_bw = data->get_bandwidth() / nchan;
  double DM = data->get_dispersion_measure();
  double time_resolution = data->get_folding_period() / nbin;
  double time_smear = 0;

  for (unsigned ichan = 0; ichan < nchan; ichan++) {
    double c_freq= data->get_centre_frequency(ichan);
    double time_smear = dispersion_smear (DM, c_freq, chan_bw);
    if (time_smear > time_resolution)
      throw Error (InvalidParam, "Pulsar::ScatteredPowerCorrection::transform",
		   "smearing in ichan=%u = %lf > time resolution = %lf",
		   ichan, time_smear, time_resolution);
  }

  JenetAnderson98 ja98;

  for (unsigned ipol = 0; ipol < npol; ipol++) {

    double mean_power = 0.0;
    unsigned ichan = 0;

    for (ichan = 0; ichan < nchan; ichan++) {

      // scattered power correction requires all channels
      if (data->get_weight(ichan) == 0.0)
	throw Error (InvalidParam,
		     "Pulsar::ScatteredPowerCorrection::transform",
		     "ichan=%u has zero weight", ipol, ichan);

      mean_power += data->get_Profile(ipol,ichan)->sum();

    }

    mean_power /= nbin;

    // cerr << "mean power = " << mean_power << endl;

    for (unsigned ibin = 0; ibin < nbin; ibin++) {

      double power = 0.0;
      
      for (ichan = 0; ichan < nchan; ichan++) {

	// scattered power correction requires all channels
	if (data->get_weight(ichan) == 0.0)
	  throw Error (InvalidParam,
		       "Pulsar::ScatteredPowerCorrection::transform",
		       "ichan=%u has zero weight", ipol, ichan);


	power += data->get_Profile(ipol,ichan)->get_amps()[ibin];

      }

      double sigma_n = power / mean_power;

#if 0
      ja98.set_sigma_n( power / mean_power );
      double A = ja98.get_A();
      //cerr << "sigma_n=" << sigma_n << " A=" << A << " <A>=0.8808" << endl;
#else
      // Table 2 of JA98
      double A = 0.8808;
#endif

      double scattered_power = power * (1-A) / nchan;

      for (ichan = 0; ichan < nchan; ichan++)
	data->get_Profile(ipol,ichan)->get_amps()[ibin] -= scattered_power;


    }

  }

}
