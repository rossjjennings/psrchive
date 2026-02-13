/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/VariableFaradayRotation.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Faraday.h"
#include "Pulsar/AuxColdPlasmaMeasures.h"

using namespace Pulsar;
using namespace std;

VariableFaradayRotation::VariableFaradayRotation ()
{
  ionospheric_rotation_measure = 0.0;
  interstellar_rotation_measure = 0.0;
  is_required = false;
  built = false;
}

LabelledJones<double> VariableFaradayRotation::get_value ()
{
  if (!built)
    build ();

  if (Archive::verbose > 1)
    cerr << "VariableFaradayRotation::get_value" << endl;

  return transformation;
}

bool VariableFaradayRotation::required () const
{
  if (!built)
    build ();

  return is_required;
}

std::string VariableFaradayRotation::get_description() const
{
  if (!built)
    build ();

  return description;
}

void VariableFaradayRotation::set_ionospheric_rotation_measure (double RM)
{
  ionospheric_rotation_measure = RM;
  built = false;
}

void VariableFaradayRotation::set_interstellar_rotation_measure (double RM)
{
  ionospheric_rotation_measure = RM;
  built = false;
}

void VariableFaradayRotation::build () const try
{
  if (built)
    return;

  const Integration* integration = archive->get_Integration (subint);

  transformation = Jones<double>::identity();
  is_required = false;
  description = "";

  if (chan >= integration->get_nchan())
    throw Error (InvalidState, "VariableFaradayRotation::build",
                 "invalid chan=%u >= nchan=%u", chan, integration->get_nchan());

  double centre_frequency = integration->get_centre_frequency(chan);

  if (centre_frequency <= 0)
  {
    description = "invalid centre frequency = " + tostring(centre_frequency);
    return;
  }

  double iono_rm = ionospheric_rotation_measure;

  if (iono_rm != 0.0)
  {
    description += " correcting ionospheric Faraday rotation - RM=" + tostring(ionospheric_rotation_measure) + "\n";
    transformation.label += " RM_iono=" + tostring(ionospheric_rotation_measure);
    is_required = true;
  }
  else if (! integration->get_absolute_rotation_corrected ())
  {
    const AuxColdPlasmaMeasures* aux = integration->get<AuxColdPlasmaMeasures> ();

    if (aux)
    {
      iono_rm = aux->get_rotation_measure();

      if (iono_rm != 0)
      {
        description += " correcting auxiliary Faraday rotation - RM=" + tostring(iono_rm) + "\n";
        transformation.label += " RM_aux=" + tostring(iono_rm);
        is_required = true;
      }
    }
  }

  if (!iono_faraday)
    iono_faraday = new Calibration::Faraday;

  iono_faraday->set_rotation_measure( iono_rm );
  // correct ionospheric Faraday rotation wrt infinite frequency
  iono_faraday->set_reference_wavelength( 0.0 );

  double ism_rm = interstellar_rotation_measure;

  if (ism_rm != 0.0)
  {
    description += " correcting interstellar Faraday rotation - (specified) RM=" + tostring(ism_rm) + "\n";
    transformation.label += " RM_ism=" + tostring(ism_rm);
    is_required = true;
  }
  else if (! archive->get_faraday_corrected ())
  {
    ism_rm = archive->get_rotation_measure ();

    if (ism_rm != 0)
    {
      description += " correcting interstellar Faraday rotation - (header) RM=" + tostring(ism_rm) + "\n";
      transformation.label += " RM_ism=" + tostring(ism_rm);
      is_required = true;
    }
  }

  if (!ism_faraday)
    ism_faraday = new Calibration::Faraday;

  ism_faraday->set_rotation_measure( ism_rm );
  // correct interstellar Faraday rotation wrt centre frequency
  ism_faraday->set_reference_frequency( archive->get_centre_frequency() );

  try {
    iono_faraday->set_frequency( integration->get_centre_frequency(chan) );
    transformation *= iono_faraday->evaluate();
  }
  catch (Error& error)
  {
    throw error += "VariableFaradayRotation::build iono_faraday";
  }

  try {
    ism_faraday->set_frequency( integration->get_centre_frequency(chan) );
    transformation *= ism_faraday->evaluate();
  }
  catch (Error& error)
  {
    throw error += "VariableFaradayRotation::build ism_faraday";
  }

  if (Archive::verbose > 1)
  {
    cerr << "VariableFaradayRotation::build description=" << description << endl;
  }

  built = true;
}
catch (Error& error)
{
  throw error += "VariableFaradayRotation::build";
}

