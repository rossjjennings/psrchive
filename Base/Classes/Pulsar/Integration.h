//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Integration.h,v $
   $Revision: 1.9 $
   $Date: 2002/04/16 01:57:36 $
   $Author: straten $ */

/*
  
  Pulsar::Integration - base class for pulsar observations

*/

#ifndef __Pulsar_Integration_h
#define __Pulsar_Integration_h

#include <vector>

#include "MJD.h"
#include "ArchiveTypes.h"

class profile;

namespace Tempo {
  class toa;
}

class Angle;
class Phase;

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
    Integration ();

    //! Destructor deletes data area
    virtual ~Integration ();

    //! Return pointer to copy of self
    virtual Integration* clone (int npol=0, int nband=0) const;

    //! Resizes the dimensions of the data area
    virtual void resize (int npol, int nband, int nbin);

    //! Call Profile::fold on every profile
    virtual void fold (int nfold);

    //! Call Profile::bsrunch on every profile
    virtual void bscrunch (int nscrunch);

    //! Integrate profiles from neighbouring bands
    virtual void fscrunch (int nscrunch = 0, bool weighted_cfreq = true);

    //! Integrate profiles from single polarizations into one total intensity
    virtual void pscrunch ();

    //! Transform from Stokes (I,Q,U,V) to the polarimetric invariant interval
    virtual void invint ();

    //! Rotate all profiles in phase to remove dispersion delays between bands
    virtual void dedisperse (double frequency = 0.0);

    //! Rotate all profiles about Stokes V axis to remove Faraday rotation
    virtual void defaraday (double rm = 0.0, double rm_iono = 0.0);

    //! Returns a single Stokes 4-vector for the given band and phase bin
    void get_Stokes (Stokes& S, int iband, int ibin) const;

    //! Returns a vector of Stokes parameters along the specified dimension
    void get_Stokes (vector<Stokes>& S, int iother,
		     Dimension::Axis abscissa = Dimension::Phase ) const;

    //! Find the transitions between high and low states in total intensity
    void find_transitions (int& hi2lo, int& lo2hi, int& buffer) const;

    //! Find the bins in which the total intensity exceeds a threshold
    void find_peak_edges (int& rise, int& fall) const;

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

    //! Computes the weighted centre frequency of an interval of sub-bands.
    double weighted_frequency (double* weight=0,
			       int band_start=0, int band_end=0) const;

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
    virtual void remove_baseline (int poln = 0, float phase = -1.0);

    //! Rotate each profile by time (in seconds)
    virtual void rotate (double time);

    //
    // snr_weight - set the weight of each profile to its snr squared
    //
    virtual void snr_weight ();
    
    virtual void Q_boost (const vector<double> & hphases);
    virtual void U_boost (const vector<double> & hphases);
    virtual void V_boost (const vector<double> & hphases);
    virtual void Q_rotation (const vector<Angle> & phases);
    virtual void U_rotation (const vector<Angle> & phases);
    virtual void V_rotation (const vector<Angle> & phases);

    //! Returns a pointer to a Profile
    Profile* get_Profile (int ipol, int iband);

    //! Returns a pointer to the vector of Profile objects for poln
    vector<Profile *>& operator[] (Poln::Measure poln);

    //! Get the number of bands
    /*! This attribute may be set only through Integration::resize */
    int get_nband () const { return nband; }

    //! Get the number of polarization measurements
    /*! This attribute may be set only through Integration::resize */
    int get_npol () const { return npol; }

    //! Get the number of bins in each profile
    /*! This attribute may be set only through Integration::resize */
    int get_nbin () const { return nbin; }
 
    //! Get the MJD at the beginning of the integration
    MJD get_mid_time() const { return mid_time; }
    //! Set the MJD at the beginning of the integration
    virtual void set_mid_time (const MJD& mjd) { mid_time = mjd; }

    //! Get the total time integrated (in seconds)
    double get_duration() const { return duration; }
    //! Set the total time integrated (in seconds)
    virtual void set_duration (double seconds) { duration = seconds; }

    //! Get the MJD at the start of the integration (convenience interface)
    MJD get_start_time () const { return mid_time - .5 * duration; }

    //! Get the MJD at the end of the integration (convenience interface)
    MJD get_end_time () const { return mid_time + .5 * duration; }

    //! Get the centre frequency (in MHz)
    double get_centre_frequency() const { return centrefreq; }
    //! Set the centre frequency (in MHz)
    virtual void set_centre_frequency (double MHz) { centrefreq = MHz; }
    
    //! Get the bandwidth (in MHz)
    double get_bandwidth() const { return bw; }
    //! Set the bandwidth (in MHz)
    virtual void set_bandwidth (double MHz) { bw = MHz; }

    //! Get the dispersion measure (in \f${\rm pc cm}^{-3}\f$)
    double get_dispersion_measure () const { return dm; }
    //! Set the dispersion measure (in \f${\rm pc cm}^{-3}\f$)
    virtual void set_dispersion_measure (double pc_cm3) { dm = pc_cm3; }
    
    //! Get the folding period (in seconds)
    double get_folding_period() const { return pfold; }
    //! Set the folding period (in seconds)
    virtual void set_folding_period (double seconds) { pfold = seconds; }

  protected:

    //! number of polarization measurments
    int npol;

    //! number of sub-bands
    int nband;
    
    //! number of bins
    int nbin;

    //! time at the middle of the observation
    MJD mid_time;

    //! duration of integration
    double duration;

    //! centre frequency (in MHz)
    double centrefreq;

    //! bandwidth (in MHz)
    double bw;

    //! folding period (in seconds)
    double pfold;

    //! dispersion measure (in \f${\rm pc cm}^{-3}\f$)
    double dm;

    //! polarimetric state of profiles
    Poln::State state;

    //! The data area
    vector< vector<Profile*> > profiles;

    //! Inherited types may re-define the type of profile contained in profiles
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
