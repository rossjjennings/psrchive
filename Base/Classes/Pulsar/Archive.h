//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Archive.h,v $
   $Revision: 1.1 $
   $Date: 2002/04/05 09:26:26 $
   $Author: straten $ */

/*
  
  Pulsar::Archive - pure virtual base class of pulsar data archives

*/

#ifndef __psrchive_h
#define __psrchive_h

namespace Pulsar {

  //
  // feeds - enumerated type for different feed configurations
  //
  enum feeds { invalid=-1, circular=0, linear=1 };

  class Archive {

  public:
    Archive ();
    virtual ~Archive ();

    // clone - dynamic copy constructor
    virtual Archive* clone ();

    // factory - dynamic constructor returns
    static Archive* factory (const char* filename);
    static Archive* factory (const string& filename);

    // //////////////////////////////////////////////////////////////////
    //
    // virtual methods - implemented by Archive
    //
    // //////////////////////////////////////////////////////////////////

    //
    // bscrunch - integrate profiles in phase
    //
    virtual void bscrunch (int nscrunch);

    //
    // fscrunch - integrate profiles in frequency
    //
    virtual void fscrunch (int nscrunch=0, bool dedisp=true);

    //
    // tscrunch - integrate profiles in time
    //
    virtual void tscrunch (int nscrunch=0, bool poly=true, bool wt=true);

    //
    // pscrunch - integrate profiles in polarization
    //
    virtual void pscrunch();

    // 
    // append - appends the sub-integrations from 'a' to 'this'
    //
    virtual void append (const Archive* a, bool check_ephemeris = true);

    //
    // centre - rotates the profiles so that pulse phase 0 is in nbin/2
    //
    virtual void centre ();

    //
    // correct - creates a new polyco and rotates profiles to align
    //
    virtual void correct();

    //
    // dedisperse - rotates the profiles to remove dispersion delays b/w bands
    //
    virtual void dedisperse (double dm = 0.0, double frequency = 0.0);

    //
    // fold - fold profiles into 1/nfold (for use with pulsars that have 
    //        more than one period across the profile)
    virtual void fold (int nfold);

    //
    // toas  - fits each profile to the standard and returns toas
    //
    virtual void toas (const Archive& standard,
		       vector<Tempo::toa>& toas, int mode=0, bool wt=false);

    //
    // deparallactify - corrects receiver feed angle orientation
    //
    virtual void deparallactify();

    //
    // parallactify - un-corrects receiver feed angle orientation
    //
    virtual void parallactify();

    //
    // ppqq - converts Stokes parameters to coherency products
    //        ie. I,Q,U,V -> PP,QQ,Re[PQ],Im[PQ]
    //
    virtual void ppqq();

    //
    // iquv - converts coherency products to Stokes parameters
    //        ie. PP,QQ,Re[PQ],Im[PQ] -> I,Q,U,V
    //
    virtual void iquv();

    //
    // invint - transforms Stokes I,Q,U,V into the polarimetric invariant 
    //          interval, Inv, where Inv*Inv = II-QQ-UU-VV
    //
    virtual void invint (bool square_root = true, // take sqrt(II-QQ-UU-VV)
			 float duty_cycle = 0.1,  // size of baseline window
			 float baseline_ph=-1);   // phase of baseline window
  
    //
    // remove_baseline - remove the baseline from all profiles
    //
    virtual void remove_baseline (float duty_cycle = 0.1,
				  int poln = 0,
				  float phase = -1.0);

    //
    // rotate - rotate each profile by time seconds
    //
    virtual void rotate (double time);

    //
    // rotate - rotate each profile by Phase
    //
    virtual void rotate (const Phase& shift);

    //
    // RM_correct - correct the Faraday rotation of Q into U
    // Assumes:  archive is in Stokes IQUV representation and that the baseline
    //           has been removed.
    virtual void RM_correct (double rotation_measure = 0, double rm_iono = 0);

    //
    // set_polyco - installs the given polyco and shifts profiles to align
    //
    virtual void set_polyco (const polyco& p, const Phase& ref_ph = Phase());

    //
    // snr_weight - set the weight of each profile to its snr squared
    //
    virtual void snr_weight (float noise_window = 1.0);

    // //////////////////////////////////////////////////////////////////
    //
    // convenience interfaces
    //
    // //////////////////////////////////////////////////////////////////

    // call bscrunch with the appropriate value
    void bscrunch_nbin (int new_nbin);
    void halvebins (int nhalve);

    // call fscrunch with the appropriate value
    void fscrunch_nband (int new_nband);

    // return the MJD at the beginning of the first sub-integration
    MJD  start_time() const;
    // return the MJD at the end of the last sub-integration
    MJD  end_time () const;

    // returns the total time integrated into all sub-integrations (s)
    double integration_length() const;

    // //////////////////////////////////////////////////////////////////
    //
    // pure virtual methods - must be implemented by children
    //
    // //////////////////////////////////////////////////////////////////

    //
    // unload - write archive to disk
    //
    virtual void unload (const char* filename) = 0;
    // convenience interface
    void unload (const string& filename) { unload (filename.c_str()); }

    //
    // load - read archive from disk
    //
    virtual void load (const char* filename) = 0;
    // convenience interface
    void load (const string& filename) { load (filename.c_str()); }


    // get the bandwidth of the observation (MHz)
    virtual double get_bandwidth () const = 0;
    virtual void   set_bandwidth (double bw) const = 0;

    // get the centre frequency of the observation (MHz)
    virtual double get_centre_frequency () const = 0;
    virtual void   set_centre_frequency (double cf) const = 0;

    // get the feed configuration of the receiver
    virtual feeds get_feed_type () const = 0;

    // get the state of various corrected flags
    virtual bool get_feedangle_corrected () const = 0;
    virtual bool get_iono_rm_corrected () const = 0;
    virtual bool get_ism_rm_corrected () const = 0;
    virtual bool get_parallactic_corrected () const = 0;

    virtual void set_feedangle_corrected (bool done = true) = 0;
    virtual void set_iono_rm_corrected (bool done = true) = 0;
    virutal void set_ism_rm_corrected (bool done = true) = 0;
    virtual void set_parallactic_corrected (bool done = true) = 0;

 protected:

    //
    // subints - the data area
    //
    vector<Integration> subints;

    // //////////////////////////////////////////////////////////////////
    //
    // virtual methods - though implemented by Archive, should generally
    //                   require redefinition
    //
    // //////////////////////////////////////////////////////////////////

    //
    // destroy - deletes all allocated memory resources
    //
    virtual void destroy ();

    //
    // init - sets all values to default
    //
    virtual void init ();

    //
    // resize - resets the dimensions of the data area
    //
    virtual void resize (int nsubint, int nband, int nbin);

    //
    // users should call ppqq and iquv to interface these routines
    //
    virtual void iq_xy();
    virtual void xy_iq();

    virtual void iv_rl();
    virtual void rl_iv();


  };

}

#endif
