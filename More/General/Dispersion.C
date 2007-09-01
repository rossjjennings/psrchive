/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Dispersion.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

Pulsar::Dispersion::Dispersion ()
{
  name = "Dispersion";
  val = "DM";
}

double Pulsar::Dispersion::correction_measure (const Integration* data)
{
  return data->get_dispersion_measure ();
}

//! Execute the correction for an entire Pulsar::Archive
void Pulsar::Dispersion::execute (Archive* arch)
{
  ColdPlasma<DispersionDelay,Dedisperse>::execute (arch);
  arch->set_dispersion_measure( get_dispersion_measure() );
  arch->set_dedispersed( true );
}

void Pulsar::Dispersion::apply (Integration* data, unsigned ichan) try
{
  folding_period = data->get_folding_period();

  for (unsigned ipol=0; ipol < data->get_npol(); ipol++)
    data->get_Profile(ipol,ichan) -> rotate_phase( get_shift() );
}
catch (Error& error) {
  throw error += "Pulsar::Dispersion::apply";
}

//! Execute the correction for an entire Pulsar::Archive
void Pulsar::Dispersion::set (const Integration* data)
{
  ColdPlasma<DispersionDelay,Dedisperse>::set (data);
  folding_period = data->get_folding_period ();
}

//! Get the phase shift
double Pulsar::Dispersion::get_shift () const
{
  if (folding_period <= 0)
    throw Error (InvalidState, "Pulsar::Dispersion::get_shift",
		 "folding period unknown");

  return (delta + corrector.evaluate()) / folding_period;
}
