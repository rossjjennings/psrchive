//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/PSRFITS/Pulsar/FITSArchive.h,v $
   $Revision: 1.14 $
   $Date: 2003/09/25 01:10:58 $
   $Author: ahotan $ */

#include <fitsio.h>

#define PSRFITS 1
#include "Pulsar/BasicArchive.h"
#include "Pulsar/FITSHdrExtension.h"
#include "Pulsar/ProcHistory.h"
#include "Pulsar/DigitiserStatistics.h"
#include "Pulsar/CalInfoExtension.h"
#include "Pulsar/ObsExtension.h"
#include "Pulsar/ITRFExtension.h"
#include "Pulsar/FrontendExtension.h"
#include "Pulsar/BackendExtension.h"
#include "Pulsar/Passband.h"
#include "Pulsar/PolnCalibratorExtension.h"

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
    
    //! Add an Extension to the Archive instance
    void add_extension (Pulsar::Archive::Extension *extension);
    
    //! Add a new row to the history, reflecting the current state
    void update_history ();
    
    //! Return a pointer to the specified extension
    const Extension* get_extension (unsigned iextension) const;
    
    //! Return a pointer to the specified extension
    Extension* get_extension (unsigned iextension);

    //! Set the dispersion measure (in \f${\rm pc cm}^{-3}\f$)
    void set_dispersion_measure (double dm);
    
    //! Get the dispersion measure (in \f${\rm pc cm}^{-3}\f$)
    double get_dispersion_measure () const;
    
  protected:

    friend class Archive::Advocate<FITSArchive>;

    //! This class registers the FITSArchive plugin class for use
    class Agent : public Archive::Advocate<FITSArchive> {

      public:

        //! Default constructor (necessary even when empty)
        Agent () {}

        //! Advocate the use of TimerArchive to interpret filename
        bool advocate (const char* filename);

        //! Return the name of the TimerArchive plugin
        string get_name () { return "FITSArchive"; }
    
        //! Return description of this plugin
        string get_description ();

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
    
    vector< Reference::To<Extension> > ev;
    
    // Useful names for simplicity of code
    ObsExtension*         obs_ext;
    FITSHdrExtension*     hdr_ext;
    ITRFExtension*        itrf_ext;
    CalInfoExtension*     cal_ext;
    FrontendExtension*    fe_ext;
    BackendExtension*     be_ext;
    ProcHistory*          history;
    DigitiserStatistics*  dstats;
    Passband*             bandpass;

    // Channel bandwidth

    double chanbw;
    
    // Instrumental peculiarities
    
    bool scale_cross_products;
    
    // Extension I/O routines

    void load_hist (fitsfile*);
    void unload_hist (fitsfile*) const;
    void load_hist_row (fitsfile*, int);
    void unload_hist_row (fitsfile*, int) const;
    
    void load_digistat (fitsfile*);
    void unload_digistat (fitsfile*) const;
    void load_digistat_row (fitsfile*, int);
    void unload_digistat_row (fitsfile*, int) const;
    
    void load_passband (fitsfile*, int);
    void unload_passband (fitsfile*, int) const;
    
    void load_pce (fitsfile*);
    void unload_pce (fitsfile*, Pulsar::PolnCalibratorExtension*) const;

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

