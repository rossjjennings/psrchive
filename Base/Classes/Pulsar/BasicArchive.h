//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/BasicArchive.h,v $
   $Revision: 1.1 $
   $Date: 2002/04/10 01:20:58 $
   $Author: ahotan $ */

# include "Archive.h"
  
namespace Pulsar {

  //! A class that defines most of the pure virtual methods declared in Archive.h
  /*!
    This class is designed to make it easier to develop new objects of type Pulsar::Archive.
    It is a minimal implimentation of the Pulsar::Archive class that defines a set of variables 
    which can be used to store the important information required by the Pulsar::Archive class.
    It also defines most of the pure virtual methods required.
    If a user wishes to work with data from a new instrument, they can start with the BasicArchive
    class and simply define the load and unload functions to perform file I/O. This should simply 
    be a matter of reading from the file and plugging information into the variables defined in 
    this class. It is possible for the user to add as many more features to their new class as 
    required.
  */

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

    //! The tempo ID character of the telescope used
    char        telcode;

    string      psrname,telid,frontend,backend,obstype,datatype;
    int         nbin,nchan,npol,nsubint;
    double      chanbw,bandwidth,cenfreq;
    float       calfreq,caldcyc,calphase;
    bool        facorr,pacorr,rm_ism,rm_iono,dedisp;

  public:

    //! A null constructor to initialize the storage variables
    BasicArchive::BasicArchive ();

    // Define the pure virtual functions defined in the ArchiveBase
    // that are used to get/set the above variables
 
    //! Return the name of the telescope used
    string get_tel_id ()
      {
	return telid;
      }

    //! Return the tempo code of the telescope used
    char get_tel_tempo_code ()
      {
	return telcode;
      }

    //! Return the name of the front-end system used
    string get_frontend_id ()
      {
	return frontend;
      }

    //! Return the name of the back-end system used
    string get_backend_id ()
      {
	return backend;
      }

    //! Return the type of observation (psr, cal, etc.)
    string get_obstype ()
      {
	return obstype;
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
    void set_obstype (string ob_type)
      {
	obstype = ob_type;
      }
    
    //! Return the bandwidth of the observation
    double get_bandwidth ()
      {
	return bandwidth;
      }
    
    //! Set the bandwidth of the observation
    void set_bandwidth (double bw)
      {
	bandwidth = bw;
      }
    
    //! Return the centre frequency of the observation
    double get_centre_frequency ()
      {
	return cenfreq;
      }
    
    //! Set the centre frequency of the observation
    void set_centre_frequency (double cf)
      {
	cenfreq = cf;
      }

    //! Return the type of feed used
    Feed::Type get_feed_type ()
      {
	return feedtype;
      }

    //! Set the type of feed used 
    void set_feed_type (Feed::Type feed)
      {
	feedtype = feed;
      }
    
    //! Return the polarisation state of the data
    Poln::State get_poln_state ()
      {
	return polstate;
      }
    
    //! Set the polarisation state of the data
    void set_poln_state (Poln::State state)
      {
	polstate = state;
      }
    
    //! Return whether or not the data has been corrected for feed angle errors
    bool get_feedangle_corrected () 
      {
	return facorr;
      }

    //! Return whether or not the data has been corrected for ionospheric Faraday rotation
    bool get_iono_rm_corrected ()
      {
	return rm_iono;
      }
    
    //! Return whether or not the data has been corrected for ISM Faraday rotation
    bool get_ism_rm_corrected ()
      {
	return rm_ism;
      }
    
    //! Return whether or not the data has been corrected for parallactic angle errors
    bool get_parallactic_corrected ()
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
    
  };

}











