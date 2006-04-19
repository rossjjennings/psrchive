/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ScatteredPowerCorrection.h"
#include "Pulsar/TwoBitStats.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Physical.h"
#include "templates.h"

#define ESTIMATE_SNR 0

//! Default constructor
Pulsar::ScatteredPowerCorrection::ScatteredPowerCorrection ()
{
}

void Pulsar::ScatteredPowerCorrection::correct (Archive* data)
{
  TwoBitStats* tbs = data->get<TwoBitStats>();

  if (tbs && tbs->get_ndig() <= 2) {

    if (Archive::verbose > 2)
      cerr << "Pulsar::ScatteredPowerCorrection::correct estimate thresholds"
	   << endl;

    unsigned ndig = tbs->get_ndig();

    thresholds.resize( ndig );

    for (unsigned idig=0; idig < ndig; idig ++) {
      vector<float> hist = tbs->get_histogram (idig);
      double mean = histomean (hist);
      ja98.set_mean_Phi( mean );
      thresholds[idig] = ja98.get_threshold();

      if (Archive::verbose > 2)
	cerr << "  mean=" << mean << " t[" << idig << "]=" << thresholds[idig]
	     << endl;
    }

  }
  else {
    thresholds.resize(0);
    ja98.set_threshold();
  }

  for (unsigned isub=0; isub < data->get_nsubint(); isub++)
    transform (data->get_Integration(isub));
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

  double maxdiff = 0.0;

  for (unsigned ipol = 0; ipol < npol; ipol++) {

    double mean_power = 0.0;
    unsigned ichan = 0;

    if (thresholds.size() == npol)
      ja98.set_threshold( thresholds[ipol] );

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

      ja98.set_sigma_n( power / mean_power );
      double A = ja98.get_A();


      // compare the estimated value of A with the theoretical value
      double A_theory = 0.8808;
      double diff = fabs(2*(A_theory-A)/(A_theory+A));
      maxdiff = max (diff, maxdiff);

      // compute the quantization noise power in each channel
      double scattered_power = power * (1-A) / nchan;

      for (ichan = 0; ichan < nchan; ichan++)
	data->get_Profile(ipol,ichan)->get_amps()[ibin] -= scattered_power;


    }

  }

  // cerr << "maxdiff=" << maxdiff << endl;
}
