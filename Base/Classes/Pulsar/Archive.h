//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Archive.h,v $
   $Revision: 1.6 $
   $Date: 2002/04/09 13:15:11 $
   $Author: ahotan $ */

/*
  
  Pulsar::Archive - pure virtual base class of pulsar data archives

*/

#ifndef __Pulsar_Archive_h
#define __Pulsar_Archive_h

#include <vector>
#include <string>

#include "MJD.h"

namespace Tempo {
  class toa;
}

class Phase;
class psrephem;
class polyco;

namespace Pulsar {

  //
  //! Different receiver feed configuration states
  //
  namespace Feed {
    enum Type { invalid=-1, Circular=0, Linear=1 };
  }

  //
  //! Different states of the integration data
  //
  namespace Poln {
    enum State { invalid, Stokes, Coherency, XXYY, Intensity, Invariant };
  }

  class Integration;

  class Archive {

  public:

    //! A verbosity flag that can be set for debugging purposes
    static bool verbose;

    Archive ();
    virtual ~Archive ();

    //
    //! Dynamic constructor returns a new instantiation of an Archive-derived child class
    /*
      \param filename
      \exception string
    */
    static Archive* factory (const char* filename);

    //! Convenience interface
    /*
      \param filename
      \exception string
    */
    static Archive* factory (const string& filename)
    { return factory (filename.c_str()); }

    //
    //! Returns a pointer to a new copy of self
    //
    virtual Archive* clone ();


    // //////////////////////////////////////////////////////////////////
    //
    // virtual methods - implemented by Archive
    //
    // //////////////////////////////////////////////////////////////////

    //
    //! Integrate pulse profiles in phase
    /*!
      \param nscrunch the number of phase bins to add together
      \exception string
    */
    virtual void bscrunch (int nscrunch);

    //
    //! Integrate profiles in frequency
    /*!
      \param nscrunch 
      \param dedisp
      \exception string
    */
    virtual void fscrunch (int nscrunch=0, bool dedisp=true);

    //
    //! Integrate profiles in time
    /*!
      \param nscrunch
      \param poly
      \param wt
      \exception string
    */
    virtual void tscrunch (int nscrunch=0, bool poly=true, bool wt=true);

    //
    //! Integrate profiles in polarization
    /*!
      \exception string
    */
    virtual void pscrunch();

    // 
    //! Appends the sub-integrations from 'a' to 'this'
    /*!
      \param a pointer to the Archive containing the new sub-ints
      \param check_ephemeris
      \exception string
    */
    virtual void append (const Archive* a, bool check_ephemeris = true);

    //
    //! Rotates the profiles so that pulse phase 0 is in nbin/2
    /*!
      \exception string
    */
    virtual void centre ();

    //
    //! Creates a new polyco and rotates profiles to align
    /*!
      \exception string
    */
    virtual void correct();

    //
    //! Rotates the profiles to remove dispersion delays b/w bands
    /*!
      \param dm the dispersion measure
      \param frequency
      \exception string
    */
    virtual void dedisperse (double dm = 0.0, double frequency = 0.0);

    //
    //! Fold profiles into 1/nfold 
    //  (for use with pulsars that have more than one period across the profile)
    /*!
      \param nfold
      \exception string
    */
    virtual void fold (int nfold);

    //
    //! Fits each profile to the standard and returns toas
    /*!
      \param standard
      \param toas
      \param mode
      \param wt
      \exception string
    */
    virtual void toas (const Archive& standard,
		       vector<Tempo::toa>& toas, int mode=0, bool wt=false);

    //
    //! Corrects receiver feed angle orientation
    /*!
      \exception string
    */
    virtual void deparallactify();

    //
    //! Un-corrects receiver feed angle orientation
    /*!
      \exception
    */
    virtual void parallactify();

    //
    //! Converts Stokes parameters to coherency products 
    //  ie. I,Q,U,V -> PP,QQ,Re[PQ],Im[PQ]
    /*!
      \exception string
    */
    virtual void ppqq();

    //
    //! Converts coherency products to Stokes parameters 
    //  ie. PP,QQ,Re[PQ],Im[PQ] -> I,Q,U,V
    /*!
      \exception string
    */
    virtual void iquv();

    //
    //! Transforms Stokes I,Q,U,V into the polarimetric invariant interval
    //  ie. Inv where Inv*Inv = II-QQ-UU-VV
    /*!
      \param square_root
      \param baseline_ph
      \exception string
    */
    virtual void invint (bool square_root = true, // take sqrt(II-QQ-UU-VV)
			 float baseline_ph=-1);   // phase of baseline window
  
    //
    //! Remove the baseline from all profiles
    /*!
      \param poln
      \param phase
      \exception string
    */
    virtual void remove_baseline (int poln = 0, float phase = -1.0);

    //
    //! Rotate each profile by time seconds
    /*!
      \param time
      \exception string
    */
    virtual void rotate (double time);

    //
    //! Rotate each profile by Phase
    /*!
      \param shift
      \exception string
    */
    virtual void rotate (const Phase& shift);

    //
    //! Correct the Faraday rotation of Q into U
    //     Assumes:  archive is in Stokes IQUV representation and 
    //               that the baseline has been removed.
    /*!
      \param rotation_measure
      \param rm_iono
    */
    virtual void RM_correct (double rotation_measure = 0, double rm_iono = 0);

    //
    //! Installs the given ephemeris, constructs a new polyco and shifts the profiles to align
    /*!
      \param e
      \exception string
    */
    virtual void set_ephem (const psrephem& e);

    //
    //! Installs the given polyco and shifts profiles to align
    /*!
      \param p
      \exception string
    */
    virtual void set_polyco (const polyco& p);


    //
    //! Set the duty cycle of the window used to calculate baseline statistics (mean, noise, etc.)
    /*!
      \param duty_cycle
      \exception string
    */
    void set_baseline_window (float duty_cycle);

    //! Set the default baseline window
    static void set_default_baseline_window (float duty_cycle);

    //
    //! Set the weight of each profile to its snr squared
    /*!
      \exception string
    */
    virtual void snr_weight ();

    // //////////////////////////////////////////////////////////////////
    //
    // convenience interfaces
    //
    // //////////////////////////////////////////////////////////////////

    //! Call bscrunch with the appropriate value
    /*!
      \param new_nbin
      \exception string
    */
    void bscrunch_nbin (int new_nbin);

    //! Halve the bins?
    /*!
      \param nhalve
      \exception string
    */
    void halvebins (int nhalve);

    //! Call fscrunch with the appropriate value
    /*!
      \param new_nband
      \exception string
    */
    void fscrunch_nband (int new_nband);

    //! Return the MJD at the beginning of the first sub-integration
    /*!
      \exception string
    */
    MJD  start_time() const;

    //! Return the MJD at the end of the last sub-integration
    /*!
      \exception string
    */
    MJD  end_time () const;

    //! Returns the total time integrated into all sub-integrations (in seconds)
    /*!
      \exception string
    */
    double integration_length() const;

    // //////////////////////////////////////////////////////////////////
    //
    // pure virtual methods - must be implemented by children
    //
    // //////////////////////////////////////////////////////////////////

    //
    //! Write archive to disk
    /*!
      \param filename the name of the file to write data to
      \exception string
    */
    virtual void unload (const char* filename) = 0;

    //! Convenience interface to the unload function
    /*!
      \param filename the name of the file to write data to
      \exception string
    */
    void unload (const string& filename) { unload (filename.c_str()); }

    //
    //! Read archive from disk
    /*!
      \param filename the name of the file to read data frrom
      \exception string
    */
    virtual void load (const char* filename) = 0;

    //! Convenience interface to the load function
    /*!
      \param filename the name of the file to read data from
      \exception string
    */
    void load (const string& filename) { load (filename.c_str()); }

    // get/set the observation vital statistics
    // ////////////////////////////////////////

    //! Get the name of the telescope used
    virtual string get_tel_id () const = 0;

    //! Get the tempo code of the telescope used
    virtual char get_tel_tempo_code () const = 0;

    //! Get the name of the frontend system used
    virtual string get_frontend_id () const = 0;

    //! Get the name of the backend system used
    virtual string get_backend_id () const =0;

    //! Get the observation type (psr, cal)
    virtual string get_obstype () const = 0;

    //! Set the name of the telescope used
    virtual string set_tel_id (string name) const = 0;

    //! Set the tempo code of the telescope used
    virtual char set_tel_tempo_code (char id_char) const = 0;

    //! Set the name of the frontend system used
    virtual string set_frontend_id (string fe_name) const = 0;

    //! Set the name of the backend system used
    virtual string set_backend_id (string be_name) const =0;

    //! Set the observation type (psr, cal)
    virtual string set_obstype (string ob_type) const = 0;

    // get/set the number of bins, bands, subints, etc
    // ///////////////////////////////////////////////

    //! Get the number of pulsar phase bins used
    virtual int get_nbins () const = 0;

    //! Get the number of frequency channels used
    virtual int get_nchan () const = 0;

    //! Get the channel bandwidth
    virtual double get_chanbw () const = 0;

    //! Get the number of sub-integrations stored in the file
    virtual int get_num_subints () const = 0;

    //! Set the number of pulsar phase bins used
    virtual int set_nbins (int numbins) const = 0;

    //! Set the number of frequency channels used
    virtual int set_nchan (int numchan) const = 0;

    //! Set the channel bandwidth
    virtual double set_chanbw (double chan_width) const = 0;

   //! Set the number of sub-integrations stored in the file
    virtual int set_num_subints (int num_sub) const = 0;

    //! Get the overall bandwidth of the observation
    virtual double get_bandwidth () const = 0;

    //! Set the overall bandwidth of the observation
    virtual void set_bandwidth (double bw) const = 0;

    //! Get the centre frequency of the observation
    virtual double get_centre_frequency () const = 0;

    //! Set the centre frequency of the observation
    virtual void set_centre_frequency (double cf) const = 0;

    //! Get the feed configuration of the receiver
    virtual Feed::Type get_feed_type () const = 0;

    //! Set the feed configuration of the receiver
    virtual void set_feed_type (Feed::Type feed) const = 0;

    //! Get the state of the profiles
    virtual Poln::State get_poln_state () const = 0;

    //! Set the state of the profiles
    virtual void set_poln_state (Poln::State state) const = 0;

    // Get the state of various corrected flags
    // //////////////////////////////////////////

    //! Return whether or not the data has been corrected for feed angle errors
    virtual bool get_feedangle_corrected () const = 0;

    //! Return whether or not the data has been corrected for ionospheric faraday rotation
    virtual bool get_iono_rm_corrected () const = 0;

    //!  Return whether or not the data has been corrected for ISM faraday rotation
    virtual bool get_ism_rm_corrected () const = 0;

    //! Return whether or not the data has been corrected for parallactic angle errors
    virtual bool get_parallactic_corrected () const = 0;

    //! Set the status of the feed angle flag
    virtual void set_feedangle_corrected (bool done = true) = 0;

    //! Set the status of the ionospheric RM flag
    virtual void set_iono_rm_corrected (bool done = true) = 0;

    //! Set the status of the ISM RM flag
    virtual void set_ism_rm_corrected (bool done = true) = 0;

    //! Set the status of the parallactic angle flag
    virtual void set_parallactic_corrected (bool done = true) = 0;


 protected:

    //
    //! The data storage area
    //
    vector<Integration*> subints;

    //
    //! Duty cycle (0.0->1.0) of the window used to calculate baseline statistics (mean, noise, etc.)
    //
    float baseline_window;
    static float default_baseline_window;


    // //////////////////////////////////////////////////////////////////
    //
    // virtual methods - though implemented by Archive, should generally
    //                   require redefinition
    //
    // //////////////////////////////////////////////////////////////////

    //
    //! Deletes all allocated memory resources
    //
    virtual void destroy ();

    //
    //! Sets all values to default
    //
    virtual void init ();

    //
    //! Resets the dimensions of the data area
    //
    virtual void resize (int nsubint, int nband=0, int npol=0, int nbin=0);

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










