//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/Timer/Pulsar/TimerArchive.h,v $
   $Revision: 1.6 $
   $Date: 2003/08/14 19:02:37 $
   $Author: straten $ */

#ifndef __Timer_Archive_h
#define __Timer_Archive_h

#include "Pulsar/Archive.h"
#include "timer.h"

namespace Pulsar {

  class TimerArchive : public Archive {

  public:

    static bool big_endian;

    //! null constructor
    TimerArchive ();

    //! copy constructor
    TimerArchive (const Archive& archive);

    //! copy constructor
    TimerArchive (const TimerArchive& archive);

    //! extraction constructor
    TimerArchive (const Archive& archive, const vector<unsigned>& subint);

    //! extraction constructor
    TimerArchive (const TimerArchive& archive, const vector<unsigned>& subint);

    //! operator =
    TimerArchive& operator= (const TimerArchive& archive);

    //! destructor
    ~TimerArchive ();

    // //////////////////////////////////////////////////////////////////
    //
    // implement the pure virtual methods of the Archive base class
    //

    //! Copy the contents of an Archive into self
    void copy (const Archive& archive, 
	       const vector<unsigned>& subints = none_selected);

    //! Returns a pointer to a new copy of self
    virtual Archive* clone () const;

    //! Return a pointer to a new archive that contains a subset of self
    virtual Archive* extract (const vector<unsigned>& subints) const;
    
    //! Get the tempo code of the telescope used
    virtual char get_telescope_code () const;
    //! Set the tempo code of the telescope used
    virtual void set_telescope_code (char telescope_code);

    //! Get the feed configuration of the receiver
    virtual Signal::Basis get_basis () const;
    //! Set the feed configuration of the receiver
    virtual void set_basis (Signal::Basis type);

    //! Get the observation type (psr, cal)
    virtual Signal::Source get_type () const;
    //! Set the observation type (psr, cal)
    virtual void set_type (Signal::Source type);

    //! Get the source name
    virtual string get_source () const;
    //! Set the source name
    virtual void set_source (const string& source);

    //! Get the receiver name
    virtual string get_receiver () const;
    //! Set the receiver name
    virtual void set_receiver (const string& rec);

    //! Get the backend name
    virtual string get_backend () const;
    //! Set the backend name
    virtual void set_backend (const string& bak);

    //! Get the coordinates of the source
    virtual sky_coord get_coordinates () const;
    //! Set the coordinates of the source
    virtual void set_coordinates (const sky_coord& coordinates);

    //! Get the number of pulsar phase bins used
    /*! This attribute may be set only through Archive::resize */
    virtual unsigned get_nbin () const;

    //! Get the number of frequency channels used
    /*! This attribute may be set only through Archive::resize */
    virtual unsigned get_nchan () const;

    //! Get the number of frequency channels used
    /*! This attribute may be set only through Archive::resize */
    virtual unsigned get_npol () const;

    //! Get the number of sub-integrations stored in the file
    /*! This attribute may be set only through Archive::resize */
    virtual unsigned get_nsubint () const;

    //! Get the overall bandwidth of the observation
    virtual double get_bandwidth () const;
    //! Set the overall bandwidth of the observation
    virtual void set_bandwidth (double bw);

    //! Get the centre frequency of the observation
    virtual double get_centre_frequency () const;
    //! Set the centre frequency of the observation
    virtual void set_centre_frequency (double cf);

    //! Get the state of the profiles
    virtual Signal::State get_state () const;
    //! Set the state of the profiles
    virtual void set_state (Signal::State state);

    //! Get the dispersion measure (in \f${\rm pc cm}^{-3}\f$)
    virtual double get_dispersion_measure () const;
    //! Set the dispersion measure (in \f${\rm pc cm}^{-3}\f$)
    virtual void set_dispersion_measure (double dm);

    //! Inter-channel dispersion delay has been removed
    virtual bool get_dedispersed () const;
    //! Set true when the inter-channel dispersion delay has been removed
    virtual void set_dedispersed (bool done = true);

    //! Data has been flux calibrated
    virtual bool get_flux_calibrated () const;
    //! Set the status of the flux calibrated flag
    virtual void set_flux_calibrated (bool done = true);

    //! Data has been poln calibrated
    virtual bool get_poln_calibrated () const;
    //! Set the status of the poln calibrated flag
    virtual void set_poln_calibrated (bool done = true);

    //! Data has been corrected for feed angle errors
    virtual bool get_feedangle_corrected () const;
    //! Set the status of the feed angle flag
    virtual void set_feedangle_corrected (bool done = true);

    //! Data has been corrected for parallactic angle errors
    virtual bool get_parallactic_corrected () const;
    //! Set the status of the parallactic angle flag
    virtual void set_parallactic_corrected (bool done = true);

    //! Data has been corrected for ionospheric faraday rotation
    virtual bool get_iono_rm_corrected () const;
    //! Set the status of the ionospheric RM flag
    virtual void set_iono_rm_corrected (bool done = true);

    //! Data has been corrected for ISM faraday rotation
    virtual bool get_ism_rm_corrected () const;
    //! Set the status of the ISM RM flag
    virtual void set_ism_rm_corrected (bool done = true);


    //! Returns the Hydra observation type, given the coordinates
    int hydra_obstype ();

    //! Set various redundant parameters in the timer and mini headers
    virtual void correct ();

  protected:

    friend class Archive::Advocate<TimerArchive>;

    class Agent : public Archive::Advocate<TimerArchive> {
    
      public:

        //! Advocate the use of TimerArchive to interpret filename
        bool advocate (const char* filename);

        //! Return the name of the TimerArchive plugin
        string get_name () { return "TimerArchive"; }

        //! Return description of this plugin
        string get_description ();

    }; 

    //! Load the header information from filename
    virtual void load_header (const char* filename);

    //! Load the specified Integration from filename, returning new instance
    virtual Integration*
    load_Integration (const char* filename, unsigned subint);

    //! Unload the Archive (header and Integration data) to filename
    virtual void unload_file (const char* filename) const;

    //! The original FPTM header information
    struct timer hdr;

    //! State of the TimerArchive attributes can be trusted
    /*! This flag is set true only when data is loaded from a file.
      Whenever a new TimerArchive is created in memory, the
      TimerArchive::correct method should be called to ensure that the
      state of all timer and mini header variables are properly set. */
    bool valid;

    //! Set the number of pulsar phase bins
    virtual void set_nbin (unsigned numbins);

    //! Set the number of frequency channels
    virtual void set_nchan (unsigned numchan);

    //! Set the number of polarization measurements
    virtual void set_npol (unsigned numpol);

    //! Set the number of sub-integrations
    virtual void set_nsubint (unsigned nsubint);

    //! The subints vector will point to TimerIntegrations
    virtual Integration* new_Integration (Integration* copy_this = 0);

    //! set code bit in hdr.corrected
    void set_corrected (int code, bool done);

    //! load the archive from an open file
    void load (FILE* fptr);

    //! unload the archive to an open file
    void unload (FILE* fptr) const;

    // loading is broken up into the following steps, so that inherited
    // types can use the building blocks to do different things

    //! load the timer header from an open file
    void hdr_load (FILE* fptr);

    //! load the backend-specific information from an open file
    virtual void backend_load (FILE* fptr);

    //! load the polyco and ephemeris from an open file
    void psr_load (FILE* fptr);

    //! load the sub-integrations from an open file
    void subint_load (FILE* fptr);

    // unloading is broken up into the following steps, so that inherited
    // types can use the building blocks to do different things

    //! unload the timer header to an open file
    void hdr_unload (FILE* fptr) const;

    //! unload the backend-specific information to an open file
    virtual void backend_unload (FILE* fptr) const;

    //! unload the polyco and ephemeris from an open file
    void psr_unload (FILE* fptr) const;

    //! unload the sub-integrations to an open file
    void subint_unload (FILE* fptr) const;

    //! Set the state of various redundant parameters in the mini headers
    void correct_Integrations ();

  };



}

#endif
