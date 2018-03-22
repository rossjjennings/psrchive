/***************************************************************************
 *
 *   Copyright (C) 2018 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ImageCorrection.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Physical.h"
#include "FTransform.h"

#include <math.h>

using namespace std;

//! Default constructor
Pulsar::ImageCorrection::ImageCorrection ()
{
  f_cent_sky = 0.0;
  f_cent_adc = 0.0;
  dir = 1;
  strict = false;
  coherent = true;
  dt[0] = dt[1] = 0.0;
  alpha[0] = alpha[1] = 0.0;
}

void Pulsar::ImageCorrection::correct (Archive* data)
{
  for (unsigned isub=0; isub < data->get_nsubint(); isub++)
    transform (data->get_Integration(isub));
}

// Smear out a profile to the given width in turns using tophat (implemented 
// as sinc in harmonic domain).  There is probably another function somewhere 
// that can do this, but whatever..
static void smear_profile(Pulsar::Profile *prof, double width)
{
  if (width==0.0) return;
  const unsigned nbin = prof->get_nbin();
  float *fprof = new float[nbin+2];
  complex<float> *cprof = (complex<float> *)fprof;
  FTransform::frc1d(nbin, fprof, prof->get_amps());
  // TODO fix up get_scale?
  //float norm = FTransform::get_scale(nbin,FTransform::frc) 
  //  * FTransform::get_scale(nbin,FTransform::bcr);
  float norm = (float)nbin;
  cprof[0] /= norm;
  for (unsigned i=1; i<nbin/2+1; i++)
    cprof[i] *= sin(M_PI*((float)i)*width) / (M_PI*((float)i)*width) / norm;
  FTransform::bcr1d(nbin, prof->get_amps(), fprof);
  delete [] fprof;
}

// Defined in Profile.C ...
void sumdiff (Pulsar::Profile* thiz, const Pulsar::Profile* that, float factor);

void Pulsar::ImageCorrection::transform (Integration* data)
{
  const unsigned nchan = data->get_nchan();
  const unsigned nbin  = data->get_nbin();
  unsigned npol  = data->get_npol();

  if (npol == 4)
  {
    if (data->get_state() != Signal::Coherence)
      throw Error (InvalidParam, "Pulsar::ImageCorrection::transform",
		   "data with npol==4 must have state=Signal::Coherence");
    npol = 2;
  }

  if (data->get_dedispersed()) 
    throw Error (InvalidState, "Pulsar::ImageCorrection::transform",
        "data have already been dedispersed");
  
  const double chan_bw = data->get_bandwidth() / nchan;
  const double DM = data->get_dispersion_measure();
  const double period = data->get_folding_period();

  if (Profile::verbose)
    cerr << "Pulsar::ImageCorrection::transform" << endl;
  
  // Make a copy of integration, smooth appropriately
  Reference::To<Integration> smoothed = data->clone();
  smoothed->orphan();
  if (coherent)
  {
    // If coherent dedisp has been applied, the imaged data will be doubly smeared,
    // once from original (negative) dispersion, then again from coherent dedisp
    // filter.  We make a copy of the data and apply the appropriate smoothing
    // here.
    for (unsigned ichan=0; ichan<smoothed->get_nchan(); ichan++) 
    {
      const double f_chan = smoothed->get_centre_frequency(ichan);
      const double f_img = f_cent_sky - (f_chan - f_cent_sky);
      const double dt = dispersion_smear(DM, f_chan, chan_bw) 
        + dispersion_smear(DM, f_img, chan_bw);
      for (unsigned ipol=0; ipol<smoothed->get_npol(); ipol++)
      {
        smear_profile(smoothed->get_Profile(ipol, ichan), dt/period);
        //for (unsigned ibin=0; ibin<smoothed->get_nbin(); ibin++)
        //  cerr << ichan << " " << ipol << " " << ibin << " " << smoothed->get_Profile(ipol, ichan)->get_amps()[ibin] << " " << data->get_Profile(ipol, ichan)->get_amps()[ibin] << endl;
      }
    }
  }
  
  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    // Channel and image frequencies
    const double f_chan = data->get_centre_frequency(ichan);
    const double f_img = f_cent_sky - (f_chan - f_cent_sky);
    const double f_bb = f_adc(f_chan);
    const int iimg = match_channel(data, f_img);

    //cerr << ichan << " " << f_chan << " " << iimg << " " << f_img << 
    //  " " << f_bb;

    // If image channel data is not present or is zapped, can't do the correction
    if ((iimg<0) || (data->get_weight(iimg)==0.0))
    {
      if (strict) data->set_weight(ichan,0.0);
      //cerr << endl;
      continue;
    }

    for (unsigned ipol=0; ipol<npol; ipol++)
    {
      // Calculate image ratio
      double r = ratio(f_adc(f_img),ipol);

      //cerr << " " << r;

      // Subtract from input data
      sumdiff(data->get_Profile(ipol,ichan), smoothed->get_Profile(ipol,iimg), -r);
    }

    //cerr << endl;
  }

}

//! See equations 2 and 3 from Kurosawa et al (2001) 
// http://ieeexplore.ieee.org/document/915383/
double Pulsar::ImageCorrection::ratio(double f, unsigned ipol) const
{
  if (f_cent_sky==0.0 || f_cent_adc==0.0)
    throw Error (InvalidState, "Pulsar::ImageCorrection::ratio",
        "f_samp and/or f_cent are not set");

  if (ipol>2)
    throw Error (InvalidState, "Pulsar:ImageCorrection::ratio",
        "ipol>2 not handled");
  
  const double s = sin(M_PI*f*dt[ipol]);
  const double s2 = s*s;
  const double c2 = 1.0 - s2;
  const double a2 = alpha[ipol]*alpha[ipol];

  return (a2*c2 + s2) / (c2 + a2*s2);
}

int Pulsar::ImageCorrection::match_channel(const Integration *subint, 
    double f_sky) const
{
  const unsigned nchan = subint->get_nchan();
  const double chbw = fabs(subint->get_bandwidth()/(double)nchan);
  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    if (fabs(subint->get_centre_frequency(ichan) - f_sky) < (0.1*chbw))
      return ichan;
  }
  return -1;
}

class Pulsar::ImageCorrection::Interface
  : public TextInterface::To<ImageCorrection>
{
public:
  Interface (ImageCorrection* instance)
  {
    if (instance)
      set_instance (instance);

    add( &ImageCorrection::get_strict,
	 &ImageCorrection::set_strict,
	 "strict", "zap uncorrectable channels" ); 

    add( &ImageCorrection::get_coherent,
	 &ImageCorrection::set_coherent,
	 "coherent", "set true if coherent dedispersion was done" ); 

    add( &ImageCorrection::get_fadc,
	 &ImageCorrection::set_fadc,
	 "fadc", "set band center frequency at samplers (MHz)");

    add( &ImageCorrection::get_fcenter,
	 &ImageCorrection::set_fcenter,
	 "fcenter", "set sky frequency of band center (MHz)" ); 

    add( &ImageCorrection::get_band_dir,
	 &ImageCorrection::set_band_dir,
	 "dir", "set band direction at sampler (+/-1)" ); 

  }
  std::string get_interface_name () const { return "img"; }
};

//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::ImageCorrection::get_interface ()
{
  return new Interface (this);
}

