/***************************************************************************
 *
 *   Copyright (C) 2006-2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FaradayRotation.h"

#include "Pulsar/Integration.h"
#include "Pulsar/Archive.h"
#include "Pulsar/PolnProfile.h"

#include "Pulsar/AuxColdPlasmaMeasures.h"
#include "Pulsar/AuxColdPlasma.h"

#include <iostream>
using namespace std;

Pulsar::FaradayRotation::FaradayRotation ()
{
  name = "FaradayRotation";
  val = "RM";
  delta = get_identity();
}

double Pulsar::FaradayRotation::get_relative_measure (const Integration* data) const
{
  if (Archive::verbose > 2)
    cerr << "Pulsar::FaradayRotation::get_relative_measure RM="
         << data->get_rotation_measure () << endl;

  return data->get_rotation_measure ();
}

double Pulsar::FaradayRotation::get_absolute_measure (const Integration* data) const
{
  const AuxColdPlasmaMeasures* aux = data->get<AuxColdPlasmaMeasures> ();
  if (!aux)
    return 0;

  return aux->get_rotation_measure ();
}

bool Pulsar::FaradayRotation::get_relative_corrected (const Integration* data) const
{
  if (Archive::verbose > 2)
    cerr << "Pulsar::FaradayRotation::get_relative_corrected corrected=" 
	       << data->get_faraday_corrected() << endl;

  return data->get_faraday_corrected();
}

bool Pulsar::FaradayRotation::get_absolute_corrected (const Integration* data) const
{
  if (Archive::verbose > 2)
    cerr << "Pulsar::DisFaradayRotationpersion::get_relative_corrected corrected=" 
	       << data->get_absolute_rotation_corrected() << endl;

  return data->get_absolute_rotation_corrected ();
}

//! Execute the correction for an entire Pulsar::Archive
void Pulsar::FaradayRotation::execute (Archive* arch)
{
  ColdPlasma<Calibration::Faraday,DeFaraday>::execute (arch);
  arch->set_rotation_measure( get_rotation_measure() );
  arch->set_faraday_corrected( true );
  arch->getadd<AuxColdPlasma>()->set_birefringence_corrected( true );
}

//! Undo the correction for an entire Pulsar::Archive
void Pulsar::FaradayRotation::revert (Archive* arch)
{
  ColdPlasma<Calibration::Faraday,DeFaraday>::revert (arch);
  arch->set_faraday_corrected( false );
}

void Pulsar::FaradayRotation::apply (Integration* data, unsigned ichan, Jones<double> rotation) try
{
  Reference::To<PolnProfile> poln_profile = data->new_PolnProfile (ichan);

  Jones<double> xform = inv(rotation);

#ifdef _DEBUG
  cerr << "Pulsar::FaradayRotation::apply ichan=" << ichan 
       << " det(xform)=" << det(xform) << endl;
#endif

  poln_profile->transform( xform );
}
catch (Error& error)
{
  throw error += "Pulsar::FaradayRotation::apply";
}

