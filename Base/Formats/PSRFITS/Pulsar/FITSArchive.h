//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/PSRFITS/Pulsar/FITSArchive.h,v $
   $Revision: 1.16 $
   $Date: 2003/10/23 00:37:00 $
   $Author: ahotan $ */

#ifndef __Pulsar_FITSArchive_h
#define __Pulsar_FITSArchive_h

#include <memory>
#include <fitsio.h>

#define PSRFITS 1
#include "Pulsar/BasicArchive.h"

#include "MJD.h"

namespace Pulsar {

  class FITSHdrExtension;
  class ObsExtension;
  class BackendExtension;
  class FrontendExtension;
  class ITRFExtension;
  class CalInfoExtension;
  class ProcHistory;
  class Passband;
  class DigitiserStatistics;
  class PolnCalibratorExtension;
 
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

    //! Add a new row to the history, reflecting the current state
    void update_history ();
    
    //! Unload FITSHdrExtension to the current HDU of the specified FITS file
    static void unload (fitsfile* fptr, const FITSHdrExtension* ext);
    
    //! Unload ObsExtension to the current HDU of the specified FITS file
    static void unload (fitsfile* fptr, const ObsExtension* ext);
    
    //! Unload FrontendExtension to the current HDU of the specified FITS file
    static void unload (fitsfile* fptr, const FrontendExtension* ext);

    //! Unload BackendExtension to the current HDU of the specified FITS file
    static void unload (fitsfile* fptr, const BackendExtension* ext);

    //! Unload ITRFExtension to the current HDU of the specified FITS file
    static void unload (fitsfile* fptr, const ITRFExtension* ext);

    //! Unload CalInfoExtension to the current HDU of the specified FITS file
    static void unload (fitsfile* fptr, const CalInfoExtension* ext);
    
    //! Unload ProcHistory to the HISTORY HDU
    static void unload (fitsfile* fptr, const ProcHistory* ext);
    
    //! Unload Passband to the BANDPASS HDU
    static void unload (fitsfile* fptr, const Passband* ext);
    
    //! Unload DigitiserStatistics to the DIG_STAT HDU
    static void unload (fitsfile* fptr, const DigitiserStatistics* ext);
    
    //! Unload PolnCalibratorExtension to the FEEDPAR HDU
    static void unload (fitsfile* fptr, const PolnCalibratorExtension* ext);
    
    
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
    
    // Channel bandwidth
    double chanbw;
    
    // Instrumental peculiarities
    bool scale_cross_products;
    
    // Reference epoch
    MJD reference_epoch;

    // Extension I/O routines
    void load_ProcHistory (fitsfile*);
    void load_DigitiserStatistics (fitsfile*);
    void load_Passband (fitsfile*);
    void load_PolnCalibratorExtension (fitsfile*);
    
    void load_ITRFExtension (fitsfile*);
    void load_CalInfoExtension (fitsfile*);

    //! Delete the HDU with the specified name
    void delete_hdu (fitsfile* fptr, char* hdu_name) const;

    // //////////////////////////////////////////////////////////////////////

    // Necessary global definitions for FITS file I/O
    
    // Helper function to write an integration to a file
    void unload_integration (int, const Integration*, fitsfile*) const;

    //! Unload Integration data to the SUBINT HDU of the specified FITS file
    void unload_integrations (fitsfile*) const;

    
   
  private:
    
    int truthval (bool) const;
    void init ();

  };

}

#endif
