//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/BasicArchive.h,v $
   $Revision: 1.27 $
   $Date: 2004/06/17 09:15:51 $
   $Author: straten $ */

#ifndef __BasicArchive_h
#define __BasicArchive_h

#include "Pulsar/Archive.h"
  
namespace Pulsar {

  //! Defines the pure virtual methods declared in Pulsar::Archive
  /*!  This class is designed to make it easier to develop new objects
    of type Pulsar::Archive.  It is a minimal implimentation of the
    Pulsar::Archive class that defines a set of variables which can be
    used to store the important information required by the
    Pulsar::Archive class.  It also defines most of the pure virtual
    methods required.  If a user wishes to work with data from a new
    instrument, they can start with the BasicArchive class and simply
    define the load and unload functions to perform file I/O. This
    should simply be a matter of reading from the file and plugging
    information into the variables defined in this class. It is
    possible for the user to add as many more features to their new
    class as required.  */

  class BasicArchive : public Archive {

  public:

    //! null constructor
    BasicArchive ();

    //! copy constructor, with option to select only some integrations
    BasicArchive (const BasicArchive& archive,
		  const vector<unsigned>& selected_subints = none_selected);

    //! copy constructor
    const BasicArchive& operator = (const BasicArchive& archive);

    //! destructor
    ~BasicArchive ();

    // //////////////////////////////////////////////////////////////////
    //
    // implement the pure virtual methods of the Archive base class
    //

    //! Get the tempo code of the telescope used
    virtual char get_telescope_code () const;
    //! Set the tempo code of the telescope used
    virtual void set_telescope_code (char telescope_code);

    //! Get the feed configuration of the receiver
    virtual Signal::Basis get_basis () const;
    //! Set the feed configuration of the receiver
    virtual void set_basis (Signal::Basis type);

    //! Get the state of the profiles
    virtual Signal::State get_state () const;
    //! Set the state of the profiles
    virtual void set_state (Signal::State state);

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

    //! Get the dispersion measure (in \f${\rm pc\, cm}^{-3}\f$)
    virtual double get_dispersion_measure () const;
    //! Set the dispersion measure (in \f${\rm pc\, cm}^{-3}\f$)
    virtual void set_dispersion_measure (double dm);

    //! Inter-channel dispersion delay has been removed
    virtual bool get_dedispersed () const;
    //! Set the status of the parallactic angle flag
    virtual void set_dedispersed (bool done = true);

    //! Get the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
    virtual double get_rotation_measure () const;
    //! Set the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
    virtual void set_rotation_measure (double rm);

    //! data has been corrected for ISM faraday rotation
    virtual bool get_ism_rm_corrected () const;
    //! Set the status of the ISM RM flag
    virtual void set_ism_rm_corrected (bool done = true);

    //! data has been corrected for ionospheric faraday rotation
    virtual bool get_iono_rm_corrected () const;
    //! Set the status of the ionospheric RM flag
    virtual void set_iono_rm_corrected (bool done = true);

    //! data has been corrected for parallactic angle errors
    virtual bool get_parallactic_corrected () const;
    //! Set the status of the parallactic angle flag
    virtual void set_parallactic_corrected (bool done = true);

    //! data has been corrected for feed angle errors
    virtual bool get_feedangle_corrected () const;
    //! Set the status of the feed angle flag
    virtual void set_feedangle_corrected (bool done = true);

    //! Data has been poln calibrated
    virtual bool get_poln_calibrated () const;
    //! Set the status of the poln calibrated flag
    virtual void set_poln_calibrated (bool done = true);

    //! Data has been flux calibrated
    virtual bool get_flux_calibrated () const;
    //! Set the status of the flux calibrated flag
    virtual void set_flux_calibrated (bool done = true);

    //! Ensure that BasicArchive code is linked
    /*! This method is called in Archive::load only to ensure that the
      BasicArchive object code is linked for use by plugin classes. */
    static void ensure_linkage ();

  protected:

    //! Set the number of pulsar phase bins used
    /*! This attribute may be set only through Archive::resize */
    virtual void set_nbin (unsigned nbin);

    //! Set the number of frequency channels used
    /*! This attribute may be set only through Archive::resize */
    virtual void set_nchan (unsigned nchan);

    //! Set the number of frequency channels used
    /*! This attribute may be set only through Archive::resize */
    virtual void set_npol (unsigned npol);

    //! Set the number of sub-integrations stored in the file
    /*! This attribute may be set only through Archive::resize */
    virtual void set_nsubint (unsigned nsubint);

    //! The tempo code of the telescope used
    char telescope_code;

    //! The feed configuration of the receiver
    Signal::Basis basis;

    //! The state of the profiles
    Signal::State state;

    //! The observation type (psr, cal)
    Signal::Source type;

    //! The source name
    string source;

    //! The receiver name
    string receiver;

    //! The backend name
    string backend;

    //! The coordinates of the source
    sky_coord coordinates;

    //! The number of pulsar phase bins used
    /*! This attribute may be set only through Archive::resize */
    unsigned nbin;

    //! The number of frequency channels used
    /*! This attribute may be set only through Archive::resize */
    unsigned nchan;

    //! The number of frequency channels used
    /*! This attribute may be set only through Archive::resize */
    unsigned npol;

    //! The number of sub-integrations stored in the file
    /*! This attribute may be set only through Archive::resize */
    unsigned nsubint;

    //! The overall bandwidth of the observation
    double bandwidth;

    //! The centre frequency of the observation
    double centre_frequency;

    //! The dispersion measure (in \f${\rm pc\, cm}^{-3}\f$)
    double dispersion_measure;

    //! The rotation measure (in \f${\rm rad\, m}^{-2}\f$)
    double rotation_measure;

    //! Data has been flux calibrated
    bool flux_calibrated;

    //! Data has been poln calibrated
    bool poln_calibrated;

    //! data has been corrected for feed angle errors
    bool feedangle_corrected;

    //! data has been corrected for ionospheric faraday rotation
    bool iono_rm_corrected;

    //! data has been corrected for ISM faraday rotation
    bool ism_rm_corrected;

    //! data has been corrected for parallactic angle errors
    bool parallactic_corrected;

    bool dedispersed;

    //! Return a pointer to a new BasicIntegration
    Integration* new_Integration (Integration* copy_this = 0);
    
  };

}

#endif

