//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/PSRFITS/Pulsar/FITSArchive.h,v $
   $Revision: 1.5 $
   $Date: 2003/06/14 01:53:31 $
   $Author: ahotan $ */

#include <fitsio.h>

#define PSRFITS 1
#include "Pulsar/BasicArchive.h"
#include "Pulsar/FITSHdrExtension.h"
#include "Pulsar/FITSHistory.h"
#include "Pulsar/CalInfoExtension.h"
#include "Pulsar/ObsExtension.h"
#include "Pulsar/ITRFExtension.h"
#include "Pulsar/FrontendExtension.h"
#include "Pulsar/BackendExtension.h"
#include "MJD.h"

namespace Pulsar {

  //! Loads and unloads Pulsar FITS archives

  /*!  This class definition provides an example of how to use the
    BasicArchive class as a building block upon which extra
    functionality can be added. It inherits the BasicArchive class and
    impliments load and unload functions to read/write data from FITS
    format files into a Pulsar::Archive class. It also defines variables 
    to store some of the extra information provided by the pulsar FITS 
    file format.
    
    The pulsar specific FITS template was developed by Dick Manchester 
    at the ATNF is used primarily with the wideband pulsar correlator at 
    the Parkes radio telescope.    
  */
  
  class FITSArchive : public BasicArchive {

  public:
    
    //! null constructor
    FITSArchive ();

    //! copy constructor
    FITSArchive (const Archive& archive);

    //! copy constructor
    FITSArchive (const FITSArchive& archive);

    //! extraction constructor
    FITSArchive (const Archive& archive, const vector<unsigned>& subints);

    //! extraction constructor
    FITSArchive (const FITSArchive& archive, const vector<unsigned>& subints);

    //! operator =
    FITSArchive& operator= (const FITSArchive& archive);

    //! destructor
    ~FITSArchive ();

    //! Copy the contents of an Archive into self
    void copy (const Archive& archive,
	       const vector<unsigned>& subints = none_selected);
    
    //! Return a new copy-constructed FITSArchive instance
    Archive* clone () const;

    //! Return a new select_copy-constructed FITSArchive instance
    Archive* extract (const vector<unsigned>& subints) const;

    //! Return the number of extensions available
    unsigned get_nextension () const;

    //! Return a pointer to the specified extension
    const Extension* get_extension (unsigned iextension) const;

  protected:

    //! This class registers the FITSArchive plugin class for use
    class Agent : public Archive::Advocate<FITSArchive> {

      public:

        Agent () { } 

        //! Advocate the use of TimerArchive to interpret filename
        bool advocate (const char* filename);

        //! Return the name of the TimerArchive plugin
        string get_name () { return "FITSArchive"; }
    
        //! Return description of this plugin
        string get_description ();

    };

    // Class for holding a row of digitiser statistics
    
    class digistat {
      
    public:
      
      digistat () { init(); }
      
      void load (fitsfile* fptr, int row);
      void unload (fitsfile* fptr, int row);
      
      ~digistat ();
      
    protected:
      
      void init ();
      
      string dig_mode;
      int ndigr;
      int nlev;
      int ncycsub;
      string diglev;

      vector<float> data;
      
    };

    // Class for holding a row of digitiser counts
    
    class digicount {
      
    public:
      
      digicount () { init(); }
      
      void load (fitsfile* fptr);
      void unload (fitsfile* fptr);
      
      ~digicount ();
      
    protected:
      
      void init ();
      
      string dig_mode;
      int ndigr;
      int nlev;
      int npthist;
      string diglev;

      vector<float> data_offsets;
      vector<float> data_scales;
      vector<int> data;
      
    };

    // Class for holding the original bandpasses
    
    class bandpass {
      
    public:
      
      bandpass () { init(); }
      
      void load (fitsfile* fptr, int nrcvr);
      void unload (fitsfile* fptr, int nrcvr);
      
      ~bandpass ();
      
    protected:
      
      void init ();
      
      int nch_orig;
      
      vector<float> data_offsets;
      vector<float> data_scales;
      vector<int> data;
      
    };
    
    //! Load the FITS header information from filename
    virtual void load_header (const char* filename);

    //! Load the specified Integration from filename, returning new instance
    virtual Integration*
    load_Integration (const char* filename, unsigned subint);

    //! Unload the FITSArchive (header and Integration data) to filename
    virtual void unload_file (const char* filename) const;

    // //////////////////////////////////////////////////////////////////////
    
    // Archive Extensions used by FITSArchive
    
    ObsExtension      obs_ext;
    FITSHdrExtension  hdr_ext;
    ITRFExtension     itrf_ext;
    CalInfoExtension  cal_ext;
    FrontendExtension fe_ext;
    BackendExtension  be_ext;
    
    // Objects to contain the information in other HDU areas
    
    //! Channel bandwidth
    double chanbw;
    
    FITSHistory* history;
    
    vector<digistat*> digitiser_statistics;
    
    digicount* digitiser_counts;
    
    bandpass* original_bandpass;
    
    // //////////////////////////////////////////////////////////////////////

    // Necessary global definitions for FITS file I/O
    
    //! Standard string length defined in fitsio.h
    char card[FLEN_CARD];    

    // Helper function to write an integration to a file
    void unload_integration (int, const Integration*, fitsfile*) const;
  
  private:

    int truthval (bool) const;
    void init ();

  };
 

}

