//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/BasicArchive.h,v $
   $Revision: 1.7 $
   $Date: 2002/04/20 10:35:27 $
   $Author: straten $ */

#include "Archive.h"
  
namespace Pulsar {

  //! A class that defines most of the pure virtual methods declared in Archive.h
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
    
    // Variables to hold the data we will be working with

  protected:

    //! The MJD at the start of the observation
    MJD         startmjd;

    //! The MJD at the end of the observation
    MJD         endmjd;

    //! The type of feed used
    Feed::Type  feedtype;

    //! The polarisation state of the data (XXYY, IQUV, etc.)
    Poln::State polstate;

    Observation::Type obstype;

    //! The tempo ID character of the telescope used
    char        telcode;

    string      psrname,telid,frontend,backend,datatype;
    int         nbin,nchan,npol,nsubint;
    double      chanbw,bandwidth,cenfreq;
    float       calfreq,caldcyc,calphase;
    bool        facorr,pacorr,rm_ism,rm_iono,dedisp;

    void init();

  public:

    // while testing, an empty load and unload
    void unload (const char * foo) { cerr << "Cannot unload " << foo << endl; }

    void load (const char * foo) { cerr << "Cannot load " << foo << endl; }

    //! Returns a pointer to a new copy of self
    Archive* clone () const { return new BasicArchive (*this); }

    //! A null constructor to initialize the storage variables
    BasicArchive () { init(); }

    //! A copy constructor
    BasicArchive (const BasicArchive& copy) { operator = (copy); }

    const BasicArchive& operator = (const BasicArchive& copy);

    //! destructor
    virtual ~BasicArchive ();

    // Define the pure virtual functions defined in the ArchiveBase
    // that are used to get/set the above variables
 
    //! Return the name of the telescope used
    string get_tel_id () const
      {
	return telid;
      }

    //! Return the tempo code of the telescope used
    char get_tel_tempo_code () const
      {
	return telcode;
      }

    //! Return the name of the front-end system used
    string get_frontend_id () const
      {
	return frontend;
      }

    //! Return the name of the back-end system used
    string get_backend_id () const
      {
	return backend;
      }

    //! Return the type of observation (psr, cal, etc.)
    Observation::Type get_observation_type () const
      {
	return obstype;
      }

    string get_source () const
    {
      return psrname;
    }

    //! Set the name of the telescope use
    void set_tel_id (string name)
      {
	telid = name;
      }

    //! Set the tempo code of the telescope used
    void set_tel_tempo_code (char id_char)
      {
	telcode = id_char;
      }

    //! Set the name of the front-end system used
    void set_frontend_id (string name)
      {
	frontend = name;
      }

    //! Set the name of the back-end system used
    void set_backend_id (string name)
      {
	backend = name;
      }

    //! Set the observation type (psr, cal etc.)
    void set_observation_type (Observation::Type ob_type)
      {
	obstype = ob_type;
      }
    
    void set_source (const string& src)
    {
      psrname = src;
    }

    //! Return the bandwidth of the observation
    double get_bandwidth () const
      {
	return bandwidth;
      }
    
    //! Set the bandwidth of the observation
    void set_bandwidth (double bw)
      {
	bandwidth = bw;
      }
    
    //! Return the centre frequency of the observation
    double get_centre_frequency () const
      {
	return cenfreq;
      }
    
    //! Set the centre frequency of the observation
    void set_centre_frequency (double cf)
      {
	cenfreq = cf;
      }

    //! Get the number of sub-integrations in the archive
    int get_nsubint () const;

    //! Set the number of sub-integrations in the archive
    void set_nsubint (int num_sub);

    //! Get the number of frequency polns used
    int get_npol () const;
    
    //! Set the number of frequency polns used
    void set_npol (int numpol);
    
    //! Get the number of frequency channels used
    int get_nchan () const;
    
    //! Set the number of frequency channels used
    void set_nchan (int numchan);

    //! Get the number of pulsar phase bins used
    int get_nbin () const;
    
    //! Set the number of pulsar phase bins used
    void set_nbin (int numbins);
    
    //! Return the type of feed used
    Feed::Type get_feed_type () const
      {
	return feedtype;
      }

    //! Set the type of feed used 
    void set_feed_type (Feed::Type feed)
      {
	feedtype = feed;
      }
    
    //! Return the polarisation state of the data
    Poln::State get_poln_state () const
      {
	return polstate;
      }
    
    //! Set the polarisation state of the data
    void set_poln_state (Poln::State state)
      {
	polstate = state;
      }
    
    //! Get the centre frequency of the observation
    virtual double get_dispersion_measure () const
    {
      return ephemeris.get_dm();
    }

    //! Set the centre frequency of the observation
    virtual void set_dispersion_measure (double dm)
    {
      ephemeris.set_dm (dm);
    }


    //! Return whether or not the data has been corrected for feed angle errors
    bool get_feedangle_corrected () const
      {
	return facorr;
      }

    //! Return whether or not the data has been corrected for ionospheric Faraday rotation
    bool get_iono_rm_corrected () const
      {
	return rm_iono;
      }
    
    //! Return whether or not the data has been corrected for ISM Faraday rotation
    bool get_ism_rm_corrected () const
      {
	return rm_ism;
      }
    
    //! Return whether or not the data has been corrected for parallactic angle errors
    bool get_parallactic_corrected () const
      {
	return pacorr;
      } 
    
    //! Set whether or not the data has been corrected for feed angle errors
    void set_feedangle_corrected (bool done = true)
      {
	facorr = done;
      }
    
    //! Set whether or not the data has been corrected for ionospheric Faraday rotation
    void set_iono_rm_corrected (bool done = true)
      {
	rm_iono = done;
      }
    
    //! Set whether or not the data has been corrected for ISM Faraday rotation
    void set_ism_rm_corrected (bool done = true)
      {
	rm_ism = done;
      }
    
    //! Set whether or not the data has been corrected for parallactic angle errors
    void set_parallactic_corrected (bool done = true)
      {
	pacorr = done;
      }
    
    //! Get the channel bandwidth
    double get_chanbw () const
    {
      return chanbw;
    }

    //! Set the channel bandwidth
    void set_chanbw (double chan_width)
    {
      chanbw = chan_width;
    }

  protected:
    //! Return a pointer to a new BasicIntegration
    Integration* new_Integration (Integration* subint);
    
  };
  

}












