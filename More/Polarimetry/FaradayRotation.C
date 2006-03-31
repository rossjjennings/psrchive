/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FaradayRotation.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"

Pulsar::FaradayRotation::FaradayRotation ()
{
  name = "FaradayRotation";
  val = "RM";
}

double Pulsar::FaradayRotation::correction_measure (Integration* data)
{
  return data->get_rotation_measure ();
}

//! Execute the correction for an entire Pulsar::Archive
void Pulsar::FaradayRotation::execute (Archive* arch)
{
  ColdPlasma<Calibration::Faraday,DeFaraday>::execute (arch);
  arch->set_rotation_measure( get_rotation_measure() );
  arch->set_faraday_corrected( true );
}

void Pulsar::FaradayRotation::apply (Integration* data, unsigned ichan) try
{
  Reference::To<PolnProfile> poln_profile = data->new_PolnProfile (ichan);
  poln_profile->transform( inv(delta * corrector.evaluate()) );
}
catch (Error& error) {
  throw error += "Pulsar::FaradayRotation::apply";
}
