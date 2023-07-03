//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/FluctSpectStats.h

#ifndef __Pulsar_FluctSpectStats_h
#define __Pulsar_FluctSpectStats_h

#include "Pulsar/Algorithm.h"
#include "Pulsar/PhaseWeight.h"

#include "FTransformAgent.h"

namespace Pulsar {

  //! Manages statistics of fluctuation spectra (Fourier transform of pulse profile)
  template<class ProfileType, class ProfileStatsType>
  class FluctSpectStats : public Algorithm
  {   
  public:

    //! Default constructor
    FluctSpectStats ();

    //! Set the Profile from which statistics will be derived
    void set_profile (const ProfileType*);

    //! Set the Profile that defines the last harmonic and baseline
    void select_profile (const ProfileType*);

    //! Set the on-pulse and baseline regions
    void set_regions (const PhaseWeight& pulse, const PhaseWeight& baseline);

    //! Set the on-pulse and baseline regions
    void get_regions (PhaseWeight& on, PhaseWeight& off) const { on = onpulse; off = baseline; }

    //! Return the last harmonic chosen in the on-pulse signal
    unsigned get_last_harmonic () const { return last_harmonic; }

    //! Get the fourier transform of the last set profile
    const ProfileType* get_fourier () const { return fourier; }

    //! Get the real component statistics
    const ProfileStatsType* get_real () const { return real; }

    //! Get the imaginary component statistics
    const ProfileStatsType* get_imag () const { return imag; }

    //! Set the fourier transform plan
    void set_plan (FTransform::Plan* p) { plan = p; }

  protected:

    //! The Profile from which statistics will be derived
    Reference::To<const ProfileType> profile;

    //! The Fourier transform of the profile
    Reference::To<ProfileType> fourier;

    //! Computes the statistics of the real component
    Reference::To<ProfileStatsType> real;

    //! Computes the statistics of the imaginary component
    Reference::To<ProfileStatsType> imag;

    //! When, true the onpulse and baseline estimators have been selected
    bool regions_set;

    PhaseWeight onpulse;
    PhaseWeight baseline;

    unsigned last_harmonic;

    //! Perform any preprocessing on fourier_transform
    virtual void preprocess (ProfileType* fourier_transform) { /* do nothing */ }

    //! Extract the reference profile
    virtual const Profile* reference (const ProfileType*) = 0;

    //! Computes the phase bin masks
    void build ();

    //! The fourier transform plan (useful in multi-threaded applications)
    FTransform::Plan* plan;

  };

}


#include "Pulsar/Fourier.h"
#include "Pulsar/LastHarmonic.h"
#include "Pulsar/BaselineEstimator.h"
#include "Pulsar/ExponentialBaseline.h"

// #define _DEBUG 1
#include "debug.h"

//! Default constructor
template<class ProfileType, class ProfileStatsType>
Pulsar::FluctSpectStats<ProfileType,ProfileStatsType>::FluctSpectStats ()
{
  real = new ProfileStatsType;
  imag = new ProfileStatsType;

  regions_set = false;
  last_harmonic = 0;
  plan = 0;
}

//! Set the Profile from which statistics will be derived
template<class ProfileType, class ProfileStatsType>
void Pulsar::FluctSpectStats<ProfileType,ProfileStatsType>::set_profile (const ProfileType* _profile)
{
  profile = _profile;
  build ();
}

//! Set the Profile from which baseline and onpulse mask will be selected
/*! It is assumed that all subsequent Profile instances passed to
  set_profile will have the same phase as set_profile */
template<class ProfileType, class ProfileStatsType>
void Pulsar::FluctSpectStats<ProfileType,ProfileStatsType>::select_profile (const ProfileType* _profile)
{
  profile = _profile;
  regions_set = false;
  build ();
  if (profile)
    regions_set = true;
}

//! Set the on-pulse and baseline regions
template<class ProfileType, class ProfileStatsType>
void Pulsar::FluctSpectStats<ProfileType,ProfileStatsType>::set_regions (const PhaseWeight& on, const PhaseWeight& off)
{
  real->set_regions (on, off);
  imag->set_regions (on, off);

  onpulse = on;
  baseline = off;

  regions_set = true;
  build ();
}

template<class ProfileType>
ProfileType* fourier_to_psd (const ProfileType* fourier)
{
  Reference::To<ProfileType> psd = fourier->clone();
  detect (psd);
  return psd.release();
}

template<class ProfileType>
ProfileType* fourier_to_psd (Reference::To<ProfileType>& fourier)
{
  return fourier_to_psd (fourier.get());
}

void copy_pad (Pulsar::PhaseWeight& into, Pulsar::PhaseWeight& from, float remainder);

template<class ProfileType, class ProfileStatsType>
void Pulsar::FluctSpectStats<ProfileType,ProfileStatsType>::build () try
{
  if (!profile)
    return;

  fourier = fourier_transform (profile, plan);
  // drop the Nyquist bin, if any
  fourier->resize( profile->get_nbin() );

  // perform any required pre-processing required by derived types
  preprocess (fourier);

  // form the power spectral density
  Reference::To<ProfileType> psd = fourier_to_psd (fourier);

  // separate fourier into real and imaginary components
  // (psd is cloned only because it has the correct number of phase bins;
  // the amplitudes are correctly set in the following for loop)
  Reference::To<ProfileType> re = psd->clone();
  Reference::To<ProfileType> im = psd->clone();

  fourier_to_re_im (fourier, re, im);

  if (!regions_set)
  {
    LastHarmonic last;
    last.set_Profile( reference(psd) );

    last.get_weight (&onpulse);
    last.get_baseline_estimator()->get_weight (&baseline);

    last_harmonic = last.get_last_harmonic();

    DEBUG("Pulsar::FluctSpectStats::build last harmonic=" << last_harmonic << " nbin on=" << onpulse.get_weight_sum());

    real->set_regions (onpulse, baseline);
    imag->set_regions (onpulse, baseline);
  }

  if (onpulse.get_nbin () != re->get_nbin())
  {
    PhaseWeight on_temp = onpulse;
    PhaseWeight off_temp = baseline;

    DEBUG("Pulsar::FluctSpectStats::build masks nbin=" << onpulse.get_nbin () << " != obs nbin=" << re->get_nbin());

    on_temp.resize( re->get_nbin() );
    off_temp.resize( re->get_nbin() );

    if (re->get_nbin() > onpulse.get_nbin ())
    {
      // the observation has more bins than the template; pad the masks
      copy_pad( on_temp, onpulse, 0 );
      copy_pad( off_temp, baseline, 1 );
    }
    else
    {
      // the observation has fewer bins than the template; 
      // therefore, the baseline of the standard might start after the last harmonic of the observation
      // therefore, compute a new baseline

      // the ExponentialBaseline is suitable for the PSD (which is distributed like chi^2 with 2 DOF)
      ExponentialBaseline baseline_estimator;
      baseline_estimator.set_Profile( reference(psd) );
      baseline_estimator.get_weight(&off_temp);
    }

    real->set_regions (on_temp, off_temp);
    imag->set_regions (on_temp, off_temp);
  }

  real->set_profile (re.release());
  imag->set_profile (im.release());
}
catch (Error& error)
{
  throw error += "Pulsar::FluctSpectStats::build";
}

#endif // __Pulsar_FluctSpectStats_h
