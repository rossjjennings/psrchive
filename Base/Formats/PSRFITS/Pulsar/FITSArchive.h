//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/PSRFITS/Pulsar/FITSArchive.h,v $
   $Revision: 1.26 $
   $Date: 2004/09/28 07:17:38 $
   $Author: ahotan $ */

#ifndef __Pulsar_FITSArchive_h
#define __Pulsar_FITSArchive_h

#include <memory>
#include <fitsio.h>

#include <algorithm>

#define PSRFITS 1
#include "Pulsar/BasicArchive.h"

#include "MJD.h"

namespace Pulsar {

  class FITSHdrExtension;
  class Pointing;
  class ObsExtension;
  class WidebandCorrelator;
  class Receiver;
  class ITRFExtension;
  class CalInfoExtension;
  class ProcHistory;
  class Passband;
  class DigitiserStatistics;
  class PolnCalibratorExtension;
  class CalibratorStokes;
 
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
    
    //! Default constructor
    FITSArchive ();

    //! Copy constructor
    FITSArchive (const FITSArchive& archive);

    //! Destructor
    ~FITSArchive ();

    //! Assignment operator
    const FITSArchive& operator= (const FITSArchive& archive);

    //! Base copy constructor
    FITSArchive (const Archive& archive);

    //! Base extraction constructor
    FITSArchive (const Archive& archive, const vector<unsigned>& subints);

    //! Copy all of the class attributes and the selected Integration data
    void copy (const Archive& archive, const vector<unsigned>& subints);

    // //////////////////////////////////////////////////////////////////
    //
    // implement the pure virtual methods of the Archive base class
    //

    //! Return a pointer to a new copy constructed instance equal to this
    FITSArchive* clone () const;

    //! Return a pointer to a new extraction constructed instance equal to this
    FITSArchive* extract (const vector<unsigned>& subints) const;

    // //////////////////////////////////////////////////////////////////
    //
    // FITSArchive specific interface
    //

    //! Add a new row to the history, reflecting the current state
    void update_history ();
    
    //! Unload FITSHdrExtension to the current HDU of the specified FITS file
    static void unload (fitsfile* fptr, const FITSHdrExtension* ext);
    
    //! Unload Pointing to the specified row of the subint table
    static void unload (fitsfile* fptr, const Pointing* ext, int row);

    //! Unload ObsExtension to the current HDU of the specified FITS file
    static void unload (fitsfile* fptr, const ObsExtension* ext);
    
    //! Unload Receiver to the current HDU of the specified FITS file
    static void unload (fitsfile* fptr, const Receiver* ext);

    //! Unload WidebandCorrelator to the current HDU of the specified FITS file
    static void unload (fitsfile* fptr, const WidebandCorrelator* ext);

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
    
     //! Unload CalibratorStokes to the CAL_POLN HDU
    static void unload (fitsfile* fptr, const CalibratorStokes* ext);
    
  protected:
    
    friend class Archive::Advocate<FITSArchive>;
    
    //! This class registers the FITSArchive plugin class for use
    class Agent : public Archive::Advocate<FITSArchive> {
      
    public:
      
      //! Default constructor (necessary even when empty)
      Agent () {}
      
      //! Advocate the use of FITSArchive to interpret filename
      bool advocate (const char* filename);
      
      //! Return the name of the FITSArchive plugin
      string get_name () { return "PSRFITS"; }
      
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
    void load_CalibratorStokes (fitsfile*);
    void load_Pointing (fitsfile* fptr, int row,
				   Pulsar::Integration* integ);
    void load_Receiver (fitsfile*);
    void load_ITRFExtension (fitsfile*);
    void load_CalInfoExtension (fitsfile*);
    void load_WidebandCorrelator (fitsfile*);

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

    vector<string> fcal_on_sources;
    vector<string> fcal_off_sources;
  };

}

#endif
