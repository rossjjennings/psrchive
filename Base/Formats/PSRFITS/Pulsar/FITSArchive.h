//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/PSRFITS/Pulsar/FITSArchive.h,v $
   $Revision: 1.2 $
   $Date: 2003/03/08 11:11:16 $
   $Author: straten $ */

#include <stdio.h>
#include <math.h>
#include <iostream>

#include <fitsio.h>

#define PSRFITS 1
#include "Pulsar/BasicArchive.h"
#include "Pulsar/BasicIntegration.h"
#include "MJD.h"
#include "sky_coord.h"
#include "genutil.h"

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

    //! Returns a new Integration instance
    virtual Integration* new_Integration (Integration* copy_this = 0);
    
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
    
    // Class for holding a row of processing history
    
    class proc_hist {
      
    public:
      
      proc_hist () { init(); }
      
      void load (fitsfile* fptr, int row);
      void unload (fitsfile* fptr, int row);
      
      ~proc_hist ();
      
      char* date_pro;
      char* proc_cmd;
      char* pol_type;
      int npol;
      int nbin;
      int nbin_prd;
      double tbin;
      double ctr_freq;
      int nchan;
      double chanbw;
      int par_corr;
      int rm_corr;
      int dedisp;
      char* sc_mthd;
      char* cal_mthd;
      char* cal_file;
      char* rfi_mthd;

    protected:
      
      void init ();
      
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

    // Read parameters that are specific to the pulsar FITS template
    
    //! Header start time (as opposed to subint start time)
    MJD start_time;
    
    //! Pulsar FITS header version information
    string hdrver;
    
    //! File creation date
    string creation_date;
    
    //! Observer name
    string observer;

    //! Project ID
    string project_ID;

    //! Name of the telescope
    string telescope;

    //! Antenna ITRF X-coordinate
    double ant_x;
    
    //! Antenna ITRF Y-coordinate
    double ant_y;
    
    //! Antenna ITRF Z-coordinate
    double ant_z;

    //! Angle of X-probe wrt platform zero
    float xpol_ang;

    //! Name of the backend configuration file used
    string configfile;
    
    //! Number of receiver channels
    int nrcvr;

    //! Coordinate mode (J2000, Gal, Ecliptic, AZEL, HADEC)
    string coordmode;
    
    //! Track mode (TRACK, SCANGC, SCANLAT)
    string trk_mode;

    //! Feed track mode - Const FA, CPA, GPA
    string fd_mode;
    
    //! Feed/Posn angle requested 
    float fa_req;

    //! Fundamental correlator cycle time
    double tcycle;
    
    //! Attenuator, Poln A
    float atten_a;

    //! Attenuator, Poln B
    float atten_b;

    //! Cal mode (OFF, SYNC, EXT1, EXT2)
    string cal_mode;

    //! Calibrator frequency
    double cal_frequency;

    //! Calibrator duty-cycle
    double cal_dutycycle;

    //! Calibrator phase
    double cal_phase;

    //! Start UT date (YYYY-MM-DD)
    string stt_date;

    //! Start UT (hh:mm:ss)
    string stt_time;

    //! Start LST
    double stt_lst;
    
    // Objects to contain the information in other HDU areas
    
    //! Channel bandwidth
    double chanbw;
    
    vector<proc_hist*> history;
    
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

