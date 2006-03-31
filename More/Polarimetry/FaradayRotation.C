/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FaradayRotation.h"
#include "Pulsar/DeFaraday.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"

Pulsar::FaradayRotation::FaradayRotation ()
{
  delta = 1.0;
}

void Pulsar::FaradayRotation::setup (Integration* data)
{
  set_rotation_measure( data->get_rotation_measure() );
  ColdPlasma::setup (data);
}
 
void Pulsar::FaradayRotation::set_rotation_measure (double rotation_measure)
{
  faraday.set_rotation_measure( rotation_measure );
}

double Pulsar::FaradayRotation::get_rotation_measure () const
{
  return faraday.get_rotation_measure().get_value();
}

//! Set the reference wavelength in metres
void Pulsar::FaradayRotation::set_reference_wavelength (double metres)
{
  faraday.set_reference_wavelength( metres );
  ColdPlasma::set_reference_wavelength( metres );
}

//! Set the rotation due to a change in reference wavelength
void Pulsar::FaradayRotation::set_delta (const Jones<double>& J)
{
  delta = J;
}

//! Get the rotation due to a change in reference wavelength
Jones<double> Pulsar::FaradayRotation::get_delta () const
{
  return delta;
}

//! Execute the correction for an entire Pulsar::Archive
void Pulsar::FaradayRotation::execute (Archive* arch)
{
  ColdPlasma::execute (arch);
  arch->set_rotation_measure( get_rotation_measure() );
  arch->set_faraday_corrected( true );
}

void Pulsar::FaradayRotation::execute (Integration* data) try
{
  double rotation_measure = get_rotation_measure();
  DeFaraday* corrected = data->get<DeFaraday>();
 
  if ( corrected ) {

    double rm = corrected->get_rotation_measure();
    double lambda = corrected->get_reference_wavelength();

    if (rm == rotation_measure && lambda == get_reference_wavelength()) {
      if (Integration::verbose)
	cerr << "Pulsar::FaradayRotation::execute data are corrected" << endl;
      return;
    }

    // calculate the rotation arising from the new centre frequency, if any
    faraday.set_wavelength( lambda );
    delta = faraday.evaluate();

    // set the effective rotation measure to the difference
    set_rotation_measure( rotation_measure - rm );

  }
  else
    delta = 1.0;

  if (Integration::verbose)
    cerr << "Pulsar::FaradayRotation::execute"
      " effective RM=" << get_rotation_measure() <<
      " reference wavelength=" << get_reference_wavelength() << endl;

  execute (data, 0, data->get_nchan());

  // restore the original rotation measure
  set_rotation_measure( rotation_measure );

  if (!corrected) {
    corrected = new DeFaraday;
    data->add_extension( corrected );
  }

  corrected->set_rotation_measure( rotation_measure );
  corrected->set_reference_wavelength( reference_wavelength );

}
catch (Error& error) {
  throw error += "Pulsar::FaradayRotation::execute";
}


/*! This worker method corrects Faraday rotation without asking many
  questions.

   \param ichan the first channel to be corrected
   \param kchan one more than the last channel to be corrected

   \pre the rotation_measure and reference_wavelength attributes will
   have been set prior to calling this method

   \pre the delta attribute will have been properly set or reset
*/
void Pulsar::FaradayRotation::execute (Integration* data,
				       unsigned ichan, unsigned kchan) try
{

  if (Integration::verbose)
    cerr << "Pulsar::FaradayRotation::execute RM=" << get_rotation_measure()
	 << " lambda_0=" << get_reference_wavelength() << " m" 
         << " delta=" << delta << endl;

  if (get_rotation_measure() == 0 && delta == 1)
    return;

  if (ichan >= data->get_nchan())
    throw Error (InvalidRange, "Pulsar::FaradayRotation::execute",
                 "start chan=%d >= nchan=%d", ichan, data->get_nchan());

  if (kchan > data->get_nchan())
    throw Error (InvalidRange, "Pulsar::FaradayRotation::execute",
                 "end chan=%d > nchan=%d", kchan, data->get_nchan());

  for (unsigned jchan=ichan; jchan < kchan; jchan++) {

    faraday.set_frequency( data->get_centre_frequency (jchan) );

    Reference::To<PolnProfile> poln_profile = data->new_PolnProfile (jchan);

    poln_profile->transform (inv( delta * faraday.evaluate() ));

  }

}
catch (Error& error) {
  throw error += "Pulsar::FaradayRotation::execute [range]";
}
