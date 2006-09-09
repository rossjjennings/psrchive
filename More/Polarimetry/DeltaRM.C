/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/DeltaRM.h"
#include "Pulsar/DeltaPA.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"

#include "Pulsar/FrequencyIntegrate.h"

#include <iostream>
using namespace std;

// #define _DEBUG 1

Pulsar::DeltaRM::DeltaRM ()
{
}

Pulsar::DeltaRM::~DeltaRM ()
{
}

//! Set the archive from which to derive the refined rotation measure
void Pulsar::DeltaRM::set_data (Archive* archive)
{
  data = archive;
  data->defaraday();
}

void get_PA (Pulsar::Integration *data, unsigned ichan,
	     vector<double>& phases, vector< Estimate<double> >& PAs,
	     float threshold);

//! Refine the rotation measure estimate
void Pulsar::DeltaRM::refine ()
{
  if (!data)
    throw Error (InvalidState, "Pulsar::DeltaRM::refine", "no data");

  Reference::To<Integration> clone = data->get_Integration(0)->clone();

  FrequencyIntegrate fscr;
  fscr.set_nchan (2);
  fscr.set_range_policy (new FrequencyIntegrate::EvenlyDistributed);
  fscr.transform (clone);

  /*
    remove the remaining Faraday rotation, so that residual delta_PA
    to be measured is (hopefully) less than 2pi 
  */
  clone->defaraday ();

  if (clone->get_nchan() != 2)
    throw Error (InvalidState, "Pulsar::DeltaRM::refine",
		 "nchan != 2 after FrequencyIntegrate");

  Reference::To<PolnProfile> profile0;
  Reference::To<PolnProfile> profile1;

  vector< Estimate<double> > posang0;
  profile0 = clone->new_PolnProfile(0);
  profile0->get_orientation (posang0, 3.0);

  vector< Estimate<double> > posang1;
  profile1 = clone->new_PolnProfile(1);
  profile1->get_orientation (posang1, 3.0);

  MeanRadian<double> mean_delta_PA;

  for (unsigned ibin=0; ibin < profile0->get_nbin(); ibin++)
    if (posang0[ibin].get_variance() && posang1[ibin].get_variance()) {

      cout << ibin << " " << posang0[ibin].get_value()
	   << " " << posang0[ibin].get_error ()
	   << " " << posang1[ibin].get_value ()
	   << " " << posang1[ibin].get_error () << endl;

      mean_delta_PA += M_PI/90.0 * (posang1[ibin] - posang0[ibin]);
    }

  double f0 = clone->get_Profile(0,0)->get_centre_frequency();
  double f1 = clone->get_Profile(0,1)->get_centre_frequency();

  // speed of light in m/s
  double speed_of_light = 299792458;

  double lambda_0 = speed_of_light / (f0 * 1e6);
  double lambda_1 = speed_of_light / (f1 * 1e6);

  cerr << "frequency_0 = " << f0 << " MHz"
    "  ->  lambda_0 = " << lambda_0 << " m" << endl
       << "frequency_1 = " << f1 << " MHz"
    "  ->  lambda_1 = " << lambda_1 << " m" << endl;

  Estimate<double> delta_PA = mean_delta_PA.get_Estimate() / 2.0;
  Estimate<double> delta_RM = delta_PA/(lambda_1*lambda_1-lambda_0*lambda_0);

  rotation_measure = delta_RM + data->get_rotation_measure();
  
  Pulsar::DeltaPA dpa;
  Estimate<double> other = dpa.get (profile0, profile1);
  cerr << "DeltaPA::get = " << other * 180.0/M_PI<< " deg." << endl;

  cerr << "delta PA = <PA_1 - PA_0> = " << 180.0/M_PI * delta_PA << " deg.\n"
       << "delta RM = " << delta_RM << endl
       << "final RM = " << rotation_measure << endl;

}
