//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Integration.h,v $
   $Revision: 1.1 $
   $Date: 2002/04/08 08:04:09 $
   $Author: straten $ */

/*
  
  Pulsar::Integration - base class for pulsar observations

*/

#ifndef __Pulsar_Integration_h
#define __Pulsar_Integration_h

#include <vector>

#include "MJD.h"

class profile;

namespace Tempo {
  class toa;
}

class Angle;
class Phase;

class Poincare;
class Stokes;


namespace Pulsar {

  class Integration  {

  public:
    static bool verbose;

    Integration ();
    virtual ~Integration ();

    //
    // clone - dynamic copy constructor
    //
    virtual Integration* clone (const Integration* copy);

    // //////////////////////////////////////////////////////////////////
    //
    // virtual methods - implemented by Integration
    //
    // //////////////////////////////////////////////////////////////////

    //
    // bscrunch - integrate profiles in phase
    //
    virtual void bscrunch (int nscrunch);

    //
    // fscrunch - integrate profiles in frequency
    //
    virtual void fscrunch (int nscrunch = 0, double dispersion_measure = 0.0);

    //
    // pscrunch - integrate profiles in polarization
    //
    virtual void pscrunch();

    //
    // baseline_levels - returns the mean and variance of the mean in the
    //                   baseline.  uses find_min or something
    //
    virtual void baseline_levels (vector<vector<double> > & mean,
				  vector<vector<double> > & varmean) const;

    //
    // dedisperse - rotates the profiles to remove dispersion delays b/w bands
    //
    virtual void dedisperse (double dm = 0.0, double frequency = 0.0);


    //
    // find_cal_transitions - finds the transitions between hi and low
    //                        in a pulsed CAL (square wave)
    //
    virtual void find_cal_transitions (int& hightolow, int& lowtohigh,
				       int& buffer);
    
    //
    // find_cal_levels - returns the mean hi/lo and variance of the mean hi/lo
    //                   for every band and poln
    //
    virtual void find_cal_levels(vector<vector<double> >&m_hi,
				 vector<vector<double> >&var_m_hi, 
				 vector<vector<double> >&m_lo, 
				 vector<vector<double> >&var_m_lo);

    virtual void find_psr_levels (vector<vector<double> >& mean_high,
				  vector<vector<double> >& mean_low);
  
    //
    // these only work for npol == 4
    //
    void  cal_levels (vector<Stokes>& hi, vector<Stokes>& lo);
    void  psr_levels (vector<Stokes>& hi, vector<Stokes>& lo);
    void  getStokes  (vector<Stokes>& S, int chan=0);

    //
    // fold - fold profiles into 1/nfold (for use with pulsars that have 
    //        more than one period across the profile)
    //
    virtual void fold (int nfold);

    //
    // invint - transforms Stokes I,Q,U,V into the polarimetric invariant 
    //          interval, Inv, where Inv*Inv = II-QQ-UU-VV
    //
    virtual void invint (bool square_root = true, // take sqrt(II-QQ-UU-VV)
			 float baseline_ph=-1);   // phase of baseline window

    //
    // adds to a vector of tempo++ toa objects
    //
    void toas (const sub_int& std_subint,
	       int nsite, const char* fname, int subint,
	       vector<Tempo::toa>& toas, int nsubchan,
	       int mode=0, bool wt=false);

    //
    // returns a toa from weighted-average over sub-channels
    //
    Tempo::toa toa (const sub_int& std_subint,
		    int nsite, const char* fname, int subint,
		    int nsubchan, int mode, bool wt);

    //
    // remove_baseline - remove the baseline from all profiles
    //
    virtual void remove_baseline (int poln = 0, float phase = -1.0);

    //
    // rotate - rotate each profile by time seconds
    //
    virtual void rotate (double time);

    //
    // rotate - rotate each profile by Phase
    //
    virtual void rotate (const Phase& shift, double period = 0.0);

    //
    // RM_correct - correct the Faraday rotation of Q into U
    // Assumes:  archive is in Stokes IQUV representation and that the baseline
    //           has been removed.
    virtual void RM_correct (double rotation_measure = 0, double rm_iono = 0);

    //
    // snr_weight - set the weight of each profile to its snr squared
    //
    virtual void snr_weight ();
    
    // return the MJD at the beginning of the integration
    MJD  start_time() const;
    // return the MJD at the end of the integration
    MJD  end_time () const;

    // returns the total time integrated (in seconds)
    double get_integration_length() const;


    virtual void Q_boost (const vector<double> & hphases);
    virtual void U_boost (const vector<double> & hphases);
    virtua  void V_boost (const vector<double> & hphases);
    virtual void Q_rotation (const vector<Angle> & phases);
    virtual void U_rotation (const vector<Angle> & phases);
    virtual void V_rotation (const vector<Angle> & phases);

  protected:

    int nbin;	               // number of bins in a profile
    int npol;	               // number of polarizations stored
    int nband;	               // number of sub-bands
    
    double centrefreq;         // Centre frequency (MHz)
    double bw;	               // Bandwidth (MHz)

    vector<vector<Profile*> profiles;

    //
    // resize - resets the dimensions of the data area
    //
    virtual void resize (int nsubint, int nband=0, int npol=0, int nbin=0);

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
