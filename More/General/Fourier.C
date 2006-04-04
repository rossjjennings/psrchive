/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Fourier.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/Profile.h"
#include "fftm.h"

Pulsar::PolnProfile* Pulsar::fourier_transform (const PolnProfile* input) try
{
  return new PolnProfile( input->get_basis(), input->get_state(),
			  fourier_transform(input->get_Profile(0)),
			  fourier_transform(input->get_Profile(1)),
			  fourier_transform(input->get_Profile(2)),
			  fourier_transform(input->get_Profile(3)) );
}
catch (Error& error) {
  throw error += "Pulsar::fourier_transform (PolnProfile)";
}

Pulsar::Profile* Pulsar::fourier_transform (const Profile* input) try
{
  Reference::To<Profile> fourier = new Profile (input->get_nbin() + 2);

  fft::frc1d (input->get_nbin(), fourier->get_amps(), input->get_amps());

  // if the FFT scales its output, rescale to compensate
  if (fft::get_normalization() == fft::nfft)
    fourier->scale( 1.0 / sqrt(double(input->get_nbin())) );

  return fourier.release();
}
catch (Error& error) {
  throw error += "Pulsar::fourier_transform (Profile)";
}

void Pulsar::detect (PolnProfile* data) try
{
  unsigned npol = 4;
  for (unsigned ipol=0; ipol<npol; ipol++)
    detect( data->get_Profile(ipol) );
}
catch (Error& error) {
  throw error += "Pulsar::detect (PolnProfile)";
}


void Pulsar::detect (Profile* data) try
{
  unsigned nbin = data->get_nbin() / 2;
  float* amps = data->get_amps();

  for (unsigned ibin=0; ibin < nbin; ibin++) {
    float re = amps[ibin*2];
    float im = amps[ibin*2+1];
    amps[ibin] = re*re + im*im;
  }

  data->resize(nbin);

  if (data->get_amps() != amps)
    throw Error (InvalidState, "Pulsar::detect (Profile)",
		 "Profile::resize does not preserve amps array");

}
catch (Error& error) {
  throw error += "Pulsar::detect (Profile)";
}
