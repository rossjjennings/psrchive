/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Dispersion.h"
#include "Pulsar/Dedisperse.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

Pulsar::Dispersion::Dispersion ()
{
  delta = 0.0;
}

void Pulsar::Dispersion::setup (Integration* data)
{
  set_dispersion_measure( data->get_dispersion_measure() );
  ColdPlasma::setup (data);
}
 
void Pulsar::Dispersion::set_dispersion_measure (double dm)
{
  dispersion.set_dispersion_measure (dm);
}

double Pulsar::Dispersion::get_dispersion_measure () const
{
  return dispersion.get_dispersion_measure ();
}

//! Set the reference wavelength in metres
void Pulsar::Dispersion::set_reference_wavelength (double metres)
{
  dispersion.set_reference_wavelength( metres );
  ColdPlasma::set_reference_wavelength( metres );
}

//! Set the dispersion due to a change in reference wavelength
void Pulsar::Dispersion::set_delta (const double delay)
{
  delta = delay;
}

//! Get the dispersion due to a change in reference wavelength
double Pulsar::Dispersion::get_delta () const
{
  return delta;
}

//! Execute the correction for an entire Pulsar::Archive
void Pulsar::Dispersion::execute (Archive* arch)
{
  ColdPlasma::execute (arch);
  arch->set_dispersion_measure( get_dispersion_measure() );
  arch->set_dedispersed( true );
}

void Pulsar::Dispersion::execute (Integration* data) try
{
  double dispersion_measure = get_dispersion_measure();
  Dedisperse* corrected = data->get<Dedisperse>();
 
  if ( corrected ) {

    double dm = corrected->get_dispersion_measure();
    double lambda = corrected->get_reference_wavelength();

    if (dm == dispersion_measure && lambda == get_reference_wavelength()) {
      if (Integration::verbose)
	cerr << "Pulsar::Dispersion::execute data are corrected" << endl;
      return;
    }

    // calculate the dispersion arising from the new centre frequency, if any
    dispersion.set_wavelength( lambda );
    delta = dispersion.get_delay ();

    // set the effective dispersion measure to the difference
    set_dispersion_measure( dispersion_measure - dm );

  }
  else
    delta = 0.0;

  if (Integration::verbose)
    cerr << "Pulsar::Dispersion::execute"
      " effective DM=" << get_dispersion_measure() <<
      " reference wavelength=" << get_reference_wavelength() << endl;

  execute (data, 0, data->get_nchan());

  // restore the original dispersion measure
  set_dispersion_measure( dispersion_measure );

  if (!corrected) {
    corrected = new Dedisperse;
    data->add_extension( corrected );
  }

  corrected->set_dispersion_measure( dispersion_measure );
  corrected->set_reference_wavelength( reference_wavelength );

}
catch (Error& error) {
  throw error += "Pulsar::Dispersion::execute";
}


/*! This worker method corrects dispersion without asking many
  questions.

   \param ichan the first channel to be corrected
   \param kchan one more than the last channel to be corrected

   \pre the dispersion_measure and reference_wavelength attributes will
   have been set prior to calling this method

   \pre the delta attribute will have been properly set or reset
*/
void Pulsar::Dispersion::execute (Integration* data,
				  unsigned ichan, unsigned kchan) try
{

  if (Integration::verbose)
    cerr << "Pulsar::Dispersion::execute DM=" << get_dispersion_measure()
	 << " lambda_0=" << get_reference_wavelength() << " m" 
         << " delta=" << delta << endl;

  if (get_dispersion_measure() == 0 && delta == 0)
    return;

  double pfold = data->get_folding_period();
  if (pfold == 0)
    throw Error (InvalidState, "Pulsar::Dispersion::execute",
		 "folding period unknown");

  if (ichan >= data->get_nchan())
    throw Error (InvalidRange, "Pulsar::Dispersion::execute",
                 "start chan=%d >= nchan=%d", ichan, data->get_nchan());

  if (kchan > data->get_nchan())
    throw Error (InvalidRange, "Pulsar::Dispersion::execute",
                 "end chan=%d > nchan=%d", kchan, data->get_nchan());

  for (unsigned jchan=ichan; jchan < kchan; jchan++) {

    dispersion.set_frequency( data->get_centre_frequency (jchan) );

    double delay = delta + dispersion.get_delay ();
    for (unsigned ipol=0; ipol < data->get_npol(); ipol++)
      data->get_Profile(ipol,jchan) -> rotate_phase (delay / pfold);

  }

}
catch (Error& error) {
  throw error += "Pulsar::Dispersion::execute [range]";
}
