//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Profile.h,v $
   $Revision: 1.25 $
   $Date: 2003/02/17 14:24:00 $
   $Author: straten $ */

#ifndef __Pulsar_Profile_h
#define __Pulsar_Profile_h

#include "toa.h"
#include "Types.h"
#include "Reference.h"

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
    
    //! null constructor produces an empty profile of zero size
    Profile () { init(); }

    //! copy constructor
    Profile (const Profile& profile) { init(); operator = (profile); }

    //! copy constructor
    Profile (const Profile* profile) { init(); operator = (*profile); }

    //! destructor destroys the data area
    virtual ~Profile ();
    
    //! returns a pointer to a new copy of self
    virtual Profile* clone ();

    //! sets profile equal to another profile
    virtual const Profile& operator = (const Profile& profile);

    //! calculates weighted average of profile and another profile
    virtual const Profile& operator += (const Profile& profile);

    //! calculates weighted average difference of profile and another profile
    virtual const Profile& operator -= (const Profile& profile);

    //! calculates the weighted sum/difference
    virtual const Profile& average (const Profile& profile, double sign);
  
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

    //! returns the bin number with the maximum amplitude
    int bin_max (int bin_start=0, int bin_end=0) const;
    //! returns the bin number with the minimum amplitude
    int bin_min (int bin_start=0, int bin_end=0) const;

    //! returns the maximum amplitude
    float max (int bin_start=0, int bin_end=0) const;
    //! returns the minimum amplitude
    float min (int bin_start=0, int bin_end=0) const;
    //! returns the sum of all amplitudes
    double sum  (int bin_start=0, int bin_end=0) const;
    //! returns the mean amplitude
    double mean (int bin_start=0, int bin_end=0) const;
    //! returns the root mean squared amplitude
    double rms  (int bin_start=0, int bin_end=0) const;

    //! Find the bin numbers at which the mean power transits
    void find_transitions (int& highlow, int& lowhigh, int& width) const;

    //! Find the bin numbers at which the cumulative power crosses thresholds
    void find_peak_edges (int& rise, int& fall) const;

    //! Returns the phase of the centre of the region with minimum mean
    float find_min_phase (float duty_cycle = default_duty_cycle) const;

    //! Returns the phase of the centre of the region with maximum mean
    float find_max_phase (float duty_cycle = default_duty_cycle) const;

    //! Returns the mean of the specified region
    double mean (float phase, float duty_cycle = default_duty_cycle,
		 double* varmean = 0) const;
    
    //! Returns the r.m.s. of the specified region
    double sigma (float phase, float duty_cycle = default_duty_cycle) const;
    
    //! Returns the signal to noise ratio of the profile
    float snr () const;

    //! Returns the signal to noise ratio by comparison with a standard
    float snr (const Profile& standard) const;

    //! Returns the signal to noise ratio by comparison with a standard
    float snr (const Profile* standard) const { return snr (*standard); }

    //! rotates the profile to remove dispersion delay
    void dedisperse (double dm, double ref_freq, double pfold);

    //! returns the signal to noise ratio of the profile
    //float snr (float duty_cycle, float wing_sigma) const;

    //float std_snr (float rms, float wing_sigma,int* st=0, int* end=0) const;

    //float snr (const Profile& std,float*, bool allow_rotate=true) const;

    //! calculates the mean, variance, and variance of the mean of the profile
    void stats (double* mean, double* variance, double* varmean=0) const
    { stats (mean, variance, varmean, 0, nbin); }

    //! calculates the mean, variance, and variance of the mean a section
    void stats (double* mean, double* variance, double* varmean,
		int istart, int iend) const;

    //! fit to the standard and return a Tempo::toa object
    Tempo::toa toa (const Profile& std, const MJD& mjd, 
		    double period, char nsite);

    //! return the shift (in turns) after fitting to the standard
    double shift (const Profile& std, float& ephase,
		  float& snrfft, float& esnrfft) const;

    //! set the world coordinates of the pgplot window and draw the profile
    void display (float phase=0, float xmin=0, float xmax=1,
		  float ymin=0, float ymax=1) const;

    //! draw the profile using pgplot
    void draw (float phase=0) const;

    //! get the number of bins
    /*! This attribute may be set only through Profile::resize */
    unsigned get_nbin () const { return nbin; }

    //! returns a pointer to the start of the array of amplitudes
    const float* get_amps () const { return amps; }
    float* get_amps () { return amps; }

    //! fills data by jumping jbin between each datum
    void get_amps (float* data, unsigned jbin) const;

#if defined(sun) && !defined(__GNUC__)
    // SUN CC 5.0 is stupid
    void set_amps (const float* data);
    void set_amps (const double* data);
    void set_amps (const int* data);
    void set_amps (const unsigned* data);
    void set_amps (const short* data);
    void set_amps (const unsigned short* data);
    void set_amps (const char* data);
    void set_amps (const unsigned char* data);
#else
    //! set the amplitudes array equal to the contents of the data array
    template <typename T> void set_amps (const T* data);

    //! set the amplitudes array equal to the contents of the data array
    template <typename T> void set_amps (const vector<T>& data);

    //! set the amplitudes array equal to the contents of the data array
    template <typename T> void get_amps (vector<T>& data);
#endif

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

    //! convolves two profiles (using fft methods)
    void fft_convolve(Profile* p1, Profile* p2); 

    //! generates a profile containing a hat function
    void hat_profile(int bin_number, int width);

    //! integrate neighbouring phase bins in profile
    virtual void bscrunch (unsigned nscrunch);

    //! integrate neighbouring sections of the profile
    virtual void fold (unsigned nfold);

    //! resize the data area
    virtual void resize (unsigned nbin);

    //! halves the number of bins like bscrunch(2^nhalve)
    virtual void halvebins (unsigned nhalve);

  protected:

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

}

#if !defined(sun) || defined(__GNUC__)

/*! 
  \param data pointer to the data elements to be copied.
  \pre data must point to at least get_nbin() elements
*/
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
void Pulsar::Profile::get_amps (vector<T>& data)
{
  data.resize (nbin);
  for (unsigned ibin=0; ibin<nbin; ibin++)
    data[ibin] = static_cast<T>( amps[ibin] );
}


#endif // bloody SUN CC 5.0 workaround

#endif // !defined __Pulsar_Profile_h
