//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Profile.h,v $
   $Revision: 1.82 $
   $Date: 2005/03/30 13:19:51 $
   $Author: straten $ */

#ifndef __Pulsar_Profile_h
#define __Pulsar_Profile_h

#include "toa.h"
#include "Types.h"
#include "Functor.h"

#include "MEAL/Gaussian.h"

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
    public methods.  This liberty may be removed in the near future.
  */
  class Profile : public Reference::Able {

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

    //! Invoke legacy toa algorithm in shift
    static bool legacy;

    //! Default fraction of maximum amplitude a 'spike' is defined to have ended at
    static float default_amplitude_dropoff;

    static unsigned ZPSF;

    //! null constructor produces an empty profile of zero size
    Profile () { init(); }

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
    virtual const Profile& operator = (const Profile& profile);

    //! calculates weighted average of profile and another profile
    virtual const Profile& operator += (const Profile& profile);

    //! calculates weighted average difference of profile and another profile
    virtual const Profile& operator -= (const Profile& profile);

    //! calculates the weighted sum/difference
    virtual const Profile& average (const Profile& profile, double sign);

    //! calculates the difference of profile and another profile after
    /*! normalising their flux */
    virtual Profile* morphological_difference (const Profile& profile,
					       double& scale, double& shift,
					       float phs1 = 0, float phs2 = 1.0);
    
    //! adds offset to each bin of the profile
    const Profile& operator += (float offset);

    //! subtracts offset from each bin of the profile
    const Profile& operator -= (float offset);

    //! multiplies each bin of the profile by scale
    const Profile& operator *= (float scale);

    //! multiplies each bin of the profile by scale
    virtual void scale (double scale);

    //! offsets each bin of the profile by offset
    virtual void offset (double offset);

    //! rotates the profile by phase (in turns)
    virtual void rotate (double phase);

    //! set all amplitudes to zero
    virtual void zero();
  
    //! calculate the signed sqrt of the absolute value of each bin 
    virtual void square_root();

    //! calculate the logarithm of each bin with value greater than threshold
    virtual void logarithm (double base = 10.0, double threshold = 0.0);

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
    //! Returns a string with an ASCII representation of the amplitudes
    string get_ascii (int bin_start=0, int bin_end=0) const;

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
    
    //! Find the bin numbers at which the flux falls below a threshold
    void find_spike_edges(int& rise, int& fall, 
			  float pc = default_amplitude_dropoff,
			  int spike_bin = -1) const;
    
    //! Sum the flux in the specified bins
    float sum_flux(int rise, int fall,
		   float min_phase = -1.0, float dc=default_duty_cycle) const;
    /*! Finding the bin numbers at which the flux falls below a threshold, 
      and sum the flux in those bins */
    //! Assumes profile is delta function
    float sum_flux(float dropoff=default_amplitude_dropoff,
		   float min_phase = -1.0, float dc=default_duty_cycle) const;

    //! Returns the bin number with the maximum amplitude
    int find_max_bin (int bin_start=0, int bin_end=0) const;
    //! Returns the bin number with the minimum amplitude
    int find_min_bin (int bin_start=0, int bin_end=0) const;

    //! The default implementation of the snr method
    static Functor<float(const Pulsar::Profile*)> snr_strategy;

    //! Returns the signal to noise ratio of the profile
    float snr () const;

    //! Returns snr based upon baseline rms and width convolved with a boxcar
    float snr_fortran(float rms);
    //! Returns integrated flux divided by the number of bins == mean flux
    float flux(float _baseline_fraction = Profile::default_duty_cycle,
	       float min_phase = -1.0);
    /*! Returns flux as above - but uses peak edges to find the profile 
      region (and an abs value sum) ONLY HI SNR profiles applicable */
    float flux_hi_snr(float _baseline_fraction = Profile::default_duty_cycle);
    /*! Returns the width of the pulse profile, at the percentage
      of the peak given by pc, where the baseline is calculated
      using a duty cycle dc */
    float width(float& error, float pc,
		float dc = Profile::default_duty_cycle) const;
    
    //! Rotates the profile to remove dispersion delay
    void dedisperse (double dm, double ref_freq, double pfold);
    
    //! The default implementation of the shift method
    static Functor<Estimate<double>(Profile, Profile)> shift_strategy;

    //! Returns the shift (in turns) between profile and standard
    Estimate<double> shift (const Profile& std) const;

    //! fit to the standard and return a Tempo::toa object
    Tempo::toa toa (const Profile& std, const MJD& mjd, 
		    double period, char nsite, string arguments = "",
		    Tempo::toa::Format fmt = Tempo::toa::Parkes) const;

    //! get the number of bins
    /*! Note that there is no set_nbin; this attribute may be set only
      through Profile::resize */
    unsigned get_nbin () const { return nbin; }

    /*! returns a vector of floats representing which bins in the
      profile are "on pulse", indicated by the presence of a '1', 
      or "off pulse", indicated by the presence of a '0' */
    vector<unsigned> get_mask () const;

    //! returns a pointer to the start of the array of amplitudes
    const float* get_amps () const { return amps; }
    float* get_amps () { return amps; }
    
    /*! returns a vector representation of the array of amplitudes,
     with all zero-weighted points cleaned out */
    vector<float> get_weighted_amps () const;
    
    //! set the amplitudes array equal to the contents of the data array
    template <typename T> void set_amps (const T* data);

    //! set the amplitudes array equal to the contents of the data array
    template <typename T> void set_amps (const vector<T>& data);

    //! set the amplitudes array equal to the contents of the data array
    template <typename T> void get_amps (vector<T>& data) const;

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
    virtual void bscrunch (unsigned nscrunch);

    //! integrate neighbouring sections of the profile
    virtual void fold (unsigned nfold);

    //! resize the data area
    virtual void resize (unsigned nbin);

    //! halves the number of bins like bscrunch(2^nhalve)
    virtual void halvebins (unsigned nhalve);
    
    //! returns a smoothed profile by whacking spectral components - takes fraction of spectrum to keep (8 == 1/8th)
    Pulsar::Profile * denoise(int fraction=8);
    void denoise_inplace(int fraction=8);

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

    //! centre frequency of profile (in MHz)
    double centrefreq;

    //! weight of profile
    float weight;

    //! polarization measure of amplitude data
    Signal::Component state;

  };

  //! Default implementation of Profile::snr method
  double snr_phase (const Profile* profile);

  //! Alternative implementation uses a fortran routine
  float snr_fortran (const Profile* profile);

  /*! Parabolic interpolation in the time domain */
  Estimate<double> ParIntShift (const Profile& std, const Profile& ephase);

  /*! Gaussian interpolation in the time domain */
  Estimate<double> GaussianShift (const Profile& std, const Profile& ephase);

  /*! Fourier domain zero-pad interpolation */
  Estimate<double> ZeroPadShift (const Profile& std, const Profile& ephase);

  /*! Fourier domain phase gradient fit */
  Estimate<double> PhaseGradShift (const Profile& std, const Profile& ephase);

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
void Pulsar::Profile::set_amps (const vector<T>& data)
{
  resize (data.size());
  for (unsigned ibin=0; ibin<nbin; ibin++)
    amps[ibin] = static_cast<float>( data[ibin] );
}

/*! 
  \param data vector of amps
*/
template <typename T>
void Pulsar::Profile::get_amps (vector<T>& data) const
{
  data.resize (nbin);
  for (unsigned ibin=0; ibin<nbin; ibin++)
    data[ibin] = static_cast<T>( amps[ibin] );
}

#endif // !defined __Pulsar_Profile_h
