//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Profile.h,v $
   $Revision: 1.2 $
   $Date: 2002/04/09 17:03:04 $
   $Author: straten $ */

#ifndef __Pulsar_Profile_h
#define __Pulsar_Profile_h

#include "toa.h"

namespace Pulsar {

  namespace Poln {
    enum Measure { invalid=-1, 
		   I, Q, U, V,
		   XX, YY, ReXY, ImXy,
		   RR, LL, ReRL, ImRL,
		   Invariant };
  }

  //! the basic observed quantity; the pulse profile
  class Profile {

  public:

    //! flag controls the amount output to stderr by Profile methods
    static bool verbose;
    
    //! null constructor produces an empty profile of zero size
    Profile ();

    //! destructor destroys the data area
    virtual ~Profile ();
    
    //! returns a pointer to a new copy of self
    virtual Profile* clone ();

    //! adds offset to each bin of the profile
    virtual void offset (float offset);

    //! multiplies each bin of the profile by scale
    virtual void scale (float scale);

    //! rotates the profile
    virtual void rotate (double nbins);

    //! integrate neighbouring phase bins in profile
    virtual void bscrunch (int nscrunch);

    //! integrate neighbouring sections of the profile
    virtual void fold (int nfold);

    //! set all amplitudes to zero
    virtual void zero();
  
    //! resize the data area
    virtual void resize (int nbin);

    //! calls bscrunch with the appropriate argument
    void halvebins (int nhalve);

    //! returns the bin number with the maximum amplitude
    int bin_max() const;
    //! returns the bin number with the minimum amplitude
    int bin_min() const;

    //! returns the maximum amplitude
    float max (int bin_start=0, int bin_end=0) const;
    //! returns the minimum amplitude
    float min (int bin_start=0, int bin_end=0) const;
    //! returns the sum of all amplitudes
    float sum (int bin_start=0, int bin_end=0) const;
    //! returns the mean amplitude
    float mean (int bin_start=0, int bin_end=0) const;
    //! returns the root mean squared amplitude
    float rms (int bin_start=0, int bin_end=0) const;

    //! find the bin numbers at which the pulsed CAL square wave switches
    void find_cal_transitions (int& highlow, int& lowhigh, int& width) const;

    //! Returns the phase of the centre of the region with minimum mean
    float find_min_phase (float duty_cycle = default_duty_cycle) const;

    //! Returns the phase of the centre of the region with maximum mean
    float find_max_phase (float duty_cycle = default_duty_cycle) const;


    //! subtracts the mean of the specified region from all bins
    void remove_mean (float phase, float duty_cycle=default_duty_cycle);

    //! calculates the mean and variance of the mean of the specified region
    void mean (float phase, double* mean, double* varmean,
	       float duty_cycle = default_duty_cycle) const;

    //! Returns the r.m.s. of the specified region
    float sigma (float phase, float duty_cycle = default_duty_cycle) const;

    //! returns the signal to noise ratio of the profile
    float snr (float duty_cycle = default_duty_cycle) const;

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

    //! fit for the shift and return a Tempo::toa object
    Tempo::toa toa (const Profile& std_prf,
		    const MJD& prf_start_time, double period, int nsite,
		    const char* fname, int isubint, int isubband, int ipol);

    static Tempo::toa toa (double phase, float ephase,
			   const MJD& prf_start_time, double freq,
			   double period, int nsite, const char* fname, 
			   int isubint, int isubband, int ipol);

  protected:

    //! fractional phase window used to find rise and fall of CAL
    static float cal_transition_window;
    static float default_duty_cycle;

    //! number of bins in the profile
    int nbin;
    //! amplitudes at each pulse phase
    float *amps;
    //! centre frequency of profile
    float centrefreq;
    //! weight of profile
    float weight;
    //! polarization measure of amplitude data
    Poln::Measure state;

  };

}

#endif
