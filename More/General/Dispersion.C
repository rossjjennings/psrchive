/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Dispersion.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

Pulsar::Dispersion::Dispersion ()
{
  name = "Dispersion";
  val = "DM";
}

double Pulsar::Dispersion::correction_measure (Integration* data)
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
  double pfold = data->get_folding_period();
  if (pfold == 0)
    throw Error (InvalidState, "Pulsar::Dispersion::execute",
		 "folding period unknown");

  double delay = delta + corrector.evaluate ();

  for (unsigned ipol=0; ipol < data->get_npol(); ipol++)
    data->get_Profile(ipol,ichan) -> rotate_phase (delay / pfold);
}
catch (Error& error) {
  throw error += "Pulsar::Dispersion::apply";
}

