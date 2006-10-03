//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Profile.h,v $
   $Revision: 1.100 $
   $Date: 2006/10/03 06:50:32 $
   $Author: straten $ */

#ifndef __Pulsar_Profile_h
#define __Pulsar_Profile_h

#include "Pulsar/Container.h"
#include "toa.h"
#include "Types.h"
#include "Functor.h"
#include "Estimate.h"

namespace Pulsar {

  //! The basic observed quantity; the pulse profile.
  /*! The Pulsar::Profile class implements a useful, yet minimal, set
    of functionality required to store, manipulate, and analyse pulsar
    profiles.  Note that:
    <UL>
    <LI> All methods that change the size of a Profile are protected.
    <LI> The Integration class is declared as a friend.
    </UL>
    This arrangement protects the size of each Profile object from
    becoming unsynchronized with the Integration in which it is
    contained.  The data in each Profile may still be manipulated through
    public methods.
  */
  class Profile : public Container {

  public:

    //! The Integration class may call protected methods
    friend class Integration;

    //! flag controls the amount output to stderr by Profile methods
    static bool verbose;

    //! When true, Profile::rotate shifts bins in the phase domain
    static bool rotate_in_phase_domain;

    //! fractional phase window used to find rise and fall of running mean
    static float transition_duty_cycle;

    //! fractional phase window used in most functions
    static float default_duty_cycle;

    //! fraction of total power used to find peak
    static float peak_edge_threshold;

    //! When true, no memory is allocated for amps
    static bool no_amps;

    //! Default constructor
    Profile (unsigned nbin = 0);

    //! copy constructor
    Profile (const Profile& profile) { init(); operator = (profile); }

    //! copy constructor
    Profile (const Profile* profile) { init(); operator = (*profile); }

    //! generates a profile containing a hat function
    Pulsar::Profile* hat_profile(int nbin, float duty_cycle);

    //! destructor destroys the data area
    virtual ~Profile ();
    
    //! returns a pointer to a new copy of self
    virtual Profile* clone () const;

    //! sets profile equal to another profile
    const Profile& operator = (const Profile& profile);

    //! calculates weighted average of profile and another profile
    const Profile& operator += (const Profile& profile);

    //! calculates weighted average difference of profile and another profile
    const Profile& operator -= (const Profile& profile);

    //! calculates the weighted sum/difference
    const Profile& average (const Profile* profile, double sign);

    //! add profile to this
    void sum (const Profile* profile);

    //! subtract profile from this
    void diff (const Profile* profile);

    //! adds offset to each bin of the profile
    const Profile& operator += (float offset);

    //! subtracts offset from each bin of the profile
    const Profile& operator -= (float offset);

    //! multiplies each bin of the profile by scale
    const Profile& operator *= (float scale);

    //! multiplies each bin of the profile by scale
    void scale (double scale);

    //! offsets each bin of the profile by offset
    void offset (double offset);

    //! rotates the profile by phase (in turns)
    void rotate_phase (double phase);

    //! set all amplitudes to zero
    void zero ();
  
    //! calculate the signed sqrt of the absolute value of each bin 
    void square_root ();

    //! calculare the absolute value of each phase bin
    void absolute ();

    //! calculate the logarithm of each bin with value greater than threshold
    void logarithm (double base = 10.0, double threshold = 0.0);

    //! Returns the maximum amplitude
    float max  (int bin_start=0, int bin_end=0) const;
    //! Returns the minimum amplitude
    float min  (int bin_start=0, int bin_end=0) const;

    //! Returns the sum of all amplitudes
    double sum (int bin_start=0, int bin_end=0) const;
    //! Returns the sum of all amplitudes squared
    double sumsq (int bin_start=0, int bin_end=0) const;
    //! Returns the sum of the absolute value
    double sumfabs (int bin_start=0, int bin_end=0) const;

    //! Calculates the mean, variance, and variance of the mean
    void stats (double* mean, double* variance = 0, double* varmean = 0,
		int bin_start=0, int bin_end=0) const;

    //! Convenience interface to stats (start_bin, end_bin)
    void stats (float phase, 
		double* mean, double* variance = 0, double* varmean = 0,
		float duty_cycle = default_duty_cycle) const;

    //! Convenience interface to stats, returns only the mean
    double mean (float phase, float duty_cycle = default_duty_cycle) const;

    //! Returns the phase of the centre of the region with maximum mean
    float find_max_phase (float duty_cycle = default_duty_cycle) const;
    //! Returns the phase of the centre of the region with minimum mean
    float find_min_phase (float duty_cycle = default_duty_cycle) const;

    //! Find the bin numbers at which the mean power transits
    void find_transitions (int& highlow, int& lowhigh, int& width) const;

    //! Find the bin numbers at which the cumulative power crosses thresholds
    void find_peak_edges (int& rise, int& fall, bool choose = true) const;
    
    //! Returns the bin number with the maximum amplitude
    int find_max_bin (int bin_start=0, int bin_end=0) const;
    //! Returns the bin number with the minimum amplitude
    int find_min_bin (int bin_start=0, int bin_end=0) const;

    //! The default implementation of the snr method
    static Functor<float(const Pulsar::Profile*)> snr_strategy;

    //! Returns the signal to noise ratio of the profile
    float snr () const;
    
    //! Rotates the profile to remove dispersion delay
    void dedisperse (double dm, double ref_freq, double pfold);
    
    //! The default implementation of the shift method
    static Functor<Estimate<double>(Profile, Profile)> shift_strategy;

    //! Returns the shift (in turns) between profile and standard
    Estimate<double> shift (const Profile& std) const;

    //! fit to the standard and return a Tempo::toa object
    Tempo::toa toa (const Profile& std, const MJD& mjd, 
		    double period, char nsite, std::string arguments = "",
		    Tempo::toa::Format fmt = Tempo::toa::Parkes) const;

    //! get the number of bins
    /*! Note that there is no set_nbin; this attribute may be set only
      through Profile::resize */
    unsigned get_nbin () const { return nbin; }

    //! returns a pointer to the start of the array of amplitudes
    const float* get_amps () const { return amps; }
    float* get_amps () { return amps; }
    
    /*! returns a vector representation of the array of amplitudes,
     with all zero-weighted points cleaned out */
    std::vector<float> get_weighted_amps () const;
    
    //! set the amplitudes array equal to the contents of the data array
    template <typename T> void set_amps (const T* data);

    //! set the amplitudes array equal to the contents of the data array
    template <typename T> void set_amps (const std::vector<T>& data);

    //! set the amplitudes array equal to the contents of the data array
    template <typename T> void get_amps (std::vector<T>& data) const;

    //! get the centre frequency (in MHz)
    double get_centre_frequency () const { return centrefreq; }
    //! set the centre frequency (in MHz)
    virtual void set_centre_frequency (double cfreq) { centrefreq = cfreq; }

    //! get the weight of the profile
    float get_weight () const { return weight; }
    //! set the weight of the profile
    virtual void set_weight (float wt) { weight = wt; }

    //! get the state of the polarization measurement
    Signal::Component get_state () const { return state; }
    //! set the state of the polarization measurement
    virtual void set_state (Signal::Component _state) { state = _state; }

    //! convolves this with the given profile (using fft method)
    void fft_convolve (const Profile* profile); 

    //! convolves this with the given profile in time domain
    void convolve (const Profile* profile); 

    //! cross-correlates this with the given profile in time domain
    void correlate (const Profile* profile); 

    //! smears this by convolution with a hat function of given duty cycle
    void smear(float duty_cycle);

    //! integrate neighbouring phase bins in profile
    void bscrunch (unsigned nscrunch);

    //! integrate neighbouring sections of the profile
    void fold (unsigned nfold);

    //! resize the data area
    virtual void resize (unsigned nbin);

    //! halves the number of bins like bscrunch(2^nhalve)
    void halvebins (unsigned nhalve);
    
    //! interface to model_profile used by Profile::shift
    void fftconv (const Profile& std, double& shift, float& eshift,
                  float& snrfft, float& esnrfft) const;

  protected:

    friend class PolnProfile;
    friend class StandardSNR;

    //! does the work for convolve and correlate
    void convolve (const Profile* profile, int direction);

    //! initializes all values to null
    void init ();

    //! number of bins in the profile
    unsigned nbin;

    //! amplitudes at each pulse phase
    float *amps;

    //! size of the amps array (always >= nbin)
    unsigned amps_size;

    //! centre frequency of profile (in MHz)
    double centrefreq;

    //! weight of profile
    float weight;

    //! polarization measure of amplitude data
    Signal::Component state;

  };

  //! Default implementation of Profile::snr method
  double snr_phase (const Profile* profile);

}


/*! 
  \param data pointer to the data elements to be copied.
  \pre data must point to at least get_nbin() elements */
template <typename T>
void Pulsar::Profile::set_amps (const T* data)
{
  for (unsigned ibin=0; ibin<nbin; ibin++)
    amps[ibin] = static_cast<float>( data[ibin] );
}

/*! 
  \param data vector of amps
*/
template <typename T>
void Pulsar::Profile::set_amps (const std::vector<T>& data)
{
  resize (data.size());
  for (unsigned ibin=0; ibin<nbin; ibin++)
    amps[ibin] = static_cast<float>( data[ibin] );
}

/*! 
  \param data vector of amps
*/
template <typename T>
void Pulsar::Profile::get_amps (std::vector<T>& data) const
{
  data.resize (nbin);
  for (unsigned ibin=0; ibin<nbin; ibin++)
    data[ibin] = static_cast<T>( amps[ibin] );
}

#endif // !defined __Pulsar_Profile_h
