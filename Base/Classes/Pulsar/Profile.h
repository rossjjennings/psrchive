//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Profile.h,v $
   $Revision: 1.50 $
   $Date: 2004/04/14 08:15:45 $
   $Author: straten $ */

#ifndef __Pulsar_Profile_h
#define __Pulsar_Profile_h

#include "toa.h"
#include "Types.h"
#include "Functor.h"

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
    /*! normalising so that their maximum amplitudes are equal */
    virtual Profile* morphological_difference (const Profile& profile,
					       bool scale_by_bins = false);
    
    //! adds offset to each bin of the profile
    virtual const Profile& operator += (float offset);

    //! subtracts offset from each bin of the profile
    virtual const Profile& operator -= (float offset);

    //! multiplies each bin of the profile by scale
    virtual const Profile& operator *= (float scale);

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
    void find_peak_edges (int& rise, int& fall) const;

    //! Returns the bin number with the maximum amplitude
    int find_max_bin (int bin_start=0, int bin_end=0) const;
    //! Returns the bin number with the minimum amplitude
    int find_min_bin (int bin_start=0, int bin_end=0) const;

    //! The functor that implements the snr method
    static Functor<double(const Pulsar::Profile*)> snr_functor;

    //! Returns the signal to noise ratio of the profile
    float snr () const;

    //! Returns the signal to noise ratio by comparison with a standard
    float snr (const Profile& standard) const;

    //! Returns the signal to noise ratio by comparison with a standard
    float snr (const Profile* standard) const { return snr (*standard); }
    
    //! Returns snr based upon rms of baseline rms and width convolved
    //! with a boxcar
    float snr_fortran(float rms);
    //! Returns integrated flux divided by the number of bins == mean flux
    float flux(float _baseline_fraction = Profile::default_duty_cycle);
    //! Returns flux as above - but uses peak edges to find the profile region (and an abs value sum) ONLY HI SNR profiles applicable
    float flux_hi_snr(float _baseline_fraction = Profile::default_duty_cycle);
    /*! Returns the width of the pulse profile, at the percentage
      of the peak given by pc, where the baseline is calculated
      using a duty cycle dc */
    float width(float pc, float dc = Profile::default_duty_cycle);
    
    //! Rotates the profile to remove dispersion delay
    void dedisperse (double dm, double ref_freq, double pfold);

    /*! find the shift relative to a standard using a time domain
      convolution and gaussian fit, returning a Tempo::toa object */
    Tempo::toa time_domain_toa (const Profile& std, const MJD& mjd, 
				double period, char nsite) const;
    
    //! fit to the standard and return a Tempo::toa object
    Tempo::toa toa (const Profile& std, const MJD& mjd, 
		    double period, char nsite) const;

    //! return the shift (in turns) after fitting to the standard
    double shift (const Profile& std, float& ephase,
		  float& snrfft, float& esnrfft) const;

    //! set the world coordinates of the pgplot window and draw the profile
    void display (float phase=0, float xmin=0, float xmax=1,
		  float ymin=0, float ymax=1, float period=1.0,
		  bool calibrated = false, bool axes = false) const;

    //! draw the profile using pgplot
    void draw (float phase=0) const;

    //! get the number of bins
    /*! This attribute may be set only through Profile::resize */
    unsigned get_nbin () const { return nbin; }

    //! returns a pointer to the start of the array of amplitudes
    const float* get_amps () const { return amps; }
    float* get_amps () { return amps; }
    
    /*! returns a pointer to the start of a weighted copy of the 
      array of amplitudes */
    vector<float> get_weighted_amps () const;
    
    //! fills data by jumping jbin between each datum
    void get_amps (float* data, unsigned jbin) const;

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

    /*! returns the value of the convolution function for a specified
      lag, calculated in the time domain */
    double tdl_convolve(const Profile* p1, int bins_to_lag) const;
    
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

  protected:

    friend class PolnProfile;

    //! does the work for convolve and correlate
    void convolve (const Profile* profile, int direction);

    //! initializes all values to null
    void init ();

    //! fractional phase window used to find rise and fall of running mean
    static float transition_duty_cycle;

    //! fractional phase window used in most functions
    static float default_duty_cycle;

    //! fraction of total power used to find peak
    static float peak_edge_threshold;

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

    //! interface to model_profile used by Profile::shift
    void fftconv (Profile& std, double& shift, float& eshift, 
		  float& snrfft, float& esnrfft);
  };

  //! Default implmentation of Profile::snr method
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
