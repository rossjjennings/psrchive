//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Integration.h,v $
   $Revision: 1.17 $
   $Date: 2002/04/22 06:59:36 $
   $Author: ahotan $ */

/*
  
  Pulsar::Integration - base class for pulsar observations

*/

#ifndef __Pulsar_Integration_h
#define __Pulsar_Integration_h

#include <vector>

#include "MJD.h"
#include "ArchiveTypes.h"

namespace Tempo {
  class toa;
}

class Stokes;

namespace Pulsar {

  class Profile;

  //! Group of Pulsar::Profile objects integrated over the same time interval
  class Integration  {

  public:
    //! flag controls the amount output to stderr by Integration methods
    static bool verbose;

    //! flag controls the behaviour of the invint function
    static bool invint_square;

    //! Null constructor simply intializes defaults
    Integration () { init(); }

    //! Destructor deletes data area
    virtual ~Integration ();

    //! Copy constructor - copies only the profiles
    Integration (const Integration& subint, int npol=0, int nchan=0);

    //! Copy the common attributes
    virtual void copy (const Integration& subint);

    //! Return the pointer to a new copy of self
    virtual Integration* clone (int npol=0, int nchan=0) const = 0;

    //! Return the pointer to a new fscrunched and pscrunched copy of self
    Integration* total () const;

    //! Resizes the dimensions of the data area
    virtual void resize (int npol=0, int nchan=0, int nbin=0);

    //! Call Profile::fold on every profile
    virtual void fold (int nfold);

    //! Call Profile::bsrunch on every profile
    virtual void bscrunch (int nscrunch);
    
    //! Rotate each profile by time (in seconds)
    virtual void rotate (double time);

    //! Integrate profiles from neighbouring chans
    virtual void fscrunch (int nscrunch = 0, bool weighted_cfreq = true);

    //! Integrate profiles from single polarizations into one total intensity
    virtual void pscrunch ();

    //! Transform from Stokes (I,Q,U,V) to the polarimetric invariant interval
    virtual void invint ();

    //! Rotate all profiles in phase to remove dispersion delays between chans
    virtual void dedisperse (double frequency = 0.0);

    //! Dedisperse only the profiles in the given channel
    virtual void dedisperse (double frequency, int chan);

    //! Rotate all profiles about Stokes V axis to remove Faraday rotation
    virtual void defaraday (double rm = 0.0, double rm_iono = 0.0);

    //! Returns a single Stokes 4-vector for the given chan and phase bin
    void get_Stokes (Stokes& S, int ichan, int ibin) const;

    //! Returns a vector of Stokes parameters along the specified dimension
    void get_Stokes (vector<Stokes>& S, int iother,
		     Dimension::Axis abscissa = Dimension::Phase ) const;

    //! Find the transitions between high and low states in total intensity
    void find_transitions (int& hi2lo, int& lo2hi, int& buffer) const;

    //! Find the bins in which the total intensity exceeds a threshold
    void find_peak_edges (int& rise, int& fall) const;

    //! Returns the centre phase of the region with maximum total intensity
    float find_max_phase () const;

    //! Returns the centre phase of the region with minimum total intensity
    float find_min_phase () const;

    //! Return the mean and variance of the mean in every profile baseline
    void baseline_levels (vector<vector<double> > & mean,
			  vector<vector<double> > & varmean) const;

    //! Returns the mean hi/lo and variance of the mean hi/lo of every profile
    void find_cal_levels (vector<vector<double> >&m_hi,
			  vector<vector<double> >&var_m_hi, 
			  vector<vector<double> >&m_lo, 
			  vector<vector<double> >&var_m_lo) const;

    void find_psr_levels (vector<vector<double> >& mean_high,
			  vector<vector<double> >& mean_low) const;

    //! Computes the weighted centre frequency of an interval of sub-chans.
    double weighted_frequency (int chan_start=0, int chan_end=0) const;

    void cal_levels (vector<Stokes>& hi, vector<Stokes>& lo) const;
    void psr_levels (vector<Stokes>& hi, vector<Stokes>& lo) const;

    //! Adds to a vector of tempo++ toa objects
    void toas (const Integration& std_subint,
	       int nsite, const char* fname, int subint,
	       vector<Tempo::toa>& toas, int nsubchan,
	       int mode=0, bool wt=false);

    //! Returns a toa from weighted-average over sub-channels
    Tempo::toa toa (const Integration& std_subint,
		    int nsite, const char* fname, int subint,
		    int nsubchan, int mode, bool wt);

    //! Remove the baseline from all profiles
    virtual void remove_baseline (float phase = -1.0);

    //! Set the weight of each profile to its snr squared
    void snr_weight ();
    
    //! Returns a pointer to the Profile given by the specified indeces
    Profile* get_Profile (int ipol, int ichan);

    //! Returns a pointer to the vector of Profile objects for poln
    vector<Profile *>& operator[] (Poln::Measure poln);

    //! Get the MJD at the start of the integration (convenience interface)
    MJD get_start_time () const;

    //! Get the MJD at the end of the integration (convenience interface)
    MJD get_end_time () const;

    //! Get the number of chans
    /*! This attribute may be set only through Integration::resize */
    virtual int get_nchan () const = 0;

    //! Get the number of polarization measurements
    /*! This attribute may be set only through Integration::resize */
    virtual int get_npol () const = 0;

    //! Get the number of bins in each profile
    /*! This attribute may be set only through Integration::resize */
    virtual int get_nbin () const = 0;
 
    //! Get the MJD at the beginning of the integration
    virtual MJD get_mid_time() const = 0;
    //! Set the MJD at the beginning of the integration
    virtual void set_mid_time (const MJD& mjd) = 0;

    //! Get the total time integrated (in seconds)
    virtual double get_duration() const =0;
    //! Set the total time integrated (in seconds)
    virtual void set_duration (double seconds) = 0;

   //! Get the centre frequency (in MHz)
    virtual double get_centre_frequency() const = 0;
    //! Set the centre frequency (in MHz)
    virtual void set_centre_frequency (double MHz) = 0;
    
    //! Get the bandwidth (in MHz)
    virtual double get_bandwidth() const = 0;
    //! Set the bandwidth (in MHz)
    virtual void set_bandwidth (double MHz) = 0;

    //! Get the dispersion measure (in \f${\rm pc cm}^{-3}\f$)
    virtual double get_dispersion_measure () const = 0;
    //! Set the dispersion measure (in \f${\rm pc cm}^{-3}\f$)
    virtual void set_dispersion_measure (double pc_cm3) = 0;
    
    //! Get the folding period (in seconds)
    virtual double get_folding_period() const = 0;
    //! Set the folding period (in seconds)
    virtual void set_folding_period (double seconds) = 0;

    //! Get the feed configuration of the receiver
    virtual Feed::Type get_feed_type () const = 0;
    //! Set the feed configuration of the receiver
    virtual void set_feed_type (Feed::Type _type) = 0;

    //! Get the polarimetric state of the profiles
    virtual Poln::State get_poln_state () const = 0;
    //! Set the polarimetric state of the profiles
    virtual void set_poln_state (Poln::State _state) = 0;


  protected:

    //! Set the number of pulsar phase bins
    /*! Called by Integration methods to update child attribute */
    virtual void set_nbin (int nbin) = 0;

    //! Set the number of frequency channels
    /*! Called by Integration methods to update child attribute */
    virtual void set_nchan (int nchan) = 0;

    //! Set the number of polarization measurements
    /*! Called by Integration methods to update child attribute */
    virtual void set_npol (int npol) = 0;

    //! The data area
    vector< vector<Profile*> > profiles;

    //! initialize null values
    void init ();

    //! All new Profile instances are created through this method
    virtual Profile* new_Profile ();

    // convert Stokes IQUV to XYUV, where X=.5(I+Q) and Y=.5(I-Q).
    // Should only be called on data with linear feeds
    void iq_xy();
    // undoes the above
    void xy_iq();
    
    // convert Stokes IQUV to RLQU, where R=.5(I+V) and L=.5(I-V).
    // Should only be called on data with circular feeds
    void iv_rl();
    // undoes the above
    void rl_iv();
    
    // these two functions do the work for the above four
    void permute (int direction);
    void convert (float fac);
    
    // sets the poln flag in the profiles appropriately
    void iquv_flag();
    void ppqq_flag();
    
    // reverse the sign of all bins, all channels, polarization = ipol
    void flip (int ipol);

  };

}

#endif
