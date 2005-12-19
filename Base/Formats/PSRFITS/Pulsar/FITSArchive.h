//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/PSRFITS/Pulsar/FITSArchive.h,v $
   $Revision: 1.32 $
   $Date: 2005/12/19 07:37:34 $
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
  class FluxCalibratorExtension;
  class CalibratorStokes;
 
  //! Loads and unloads PSRFITS archives

  /*!  This class implements load and unload functions to read/write data
    from PSRFITS files into an Archive class. It also defines variables to
    store some of the extra information provided by the PSRFITS file
    format.
    
    PSRFITS template was developed by Dick Manchester and collaborators at
    the ATNF, ASTRON, and Swinburne.  It is used primarily with the wideband
    pulsar correlator at the Parkes radio telescope.
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
    
    //! Unload FluxCalibratorExtension to the FLUX_CAL HDU
    static void unload (fitsfile* fptr, const FluxCalibratorExtension* ext);

     //! Unload CalibratorStokes to the CAL_POLN HDU
    static void unload (fitsfile* fptr, const CalibratorStokes* ext);
    
  protected:
    
    friend class Archive::Advocate<FITSArchive>;
    
    //! Advocates the use of the FITSArchive plugin
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
    
    // Channel bandwidth
    double chanbw;
    
    // Double cross coherence term
    bool scale_cross_products;

    // Complex conjugate cross coherence term
    bool conjugate_cross_products;

    // Archive Extensions used by FITSArchive
    
    // Extension I/O routines
    void load_ProcHistory (fitsfile*);
    void load_DigitiserStatistics (fitsfile*);
    void load_Passband (fitsfile*);
    void load_PolnCalibratorExtension (fitsfile*);
    void load_FluxCalibratorExtension (fitsfile*);
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

    //! Extra polyco information
    FITSPolyco extra_polyco;

    //! The polyco parsed from the PSRFITS file
    Reference::To<polyco> hdr_model;

  private:

    // Correct the reference epoch in WBC data taken during commissioning
    void P236_reference_epoch_correction ();

    // Correct the reference epoch in WBC data taken during commissioning
    bool correct_P236_reference_epoch;

   int truthval (bool) const;
    void init ();

    vector<string> fcal_on_sources;
    vector<string> fcal_off_sources;
  };

}

#endif
