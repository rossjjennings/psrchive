//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Formats/PSRFITS/Pulsar/FITSArchive.h

#ifndef __Pulsar_FITSArchive_h
#define __Pulsar_FITSArchive_h

#include "Pulsar/BasicArchive.h"
#include "Pulsar/Agent.h"

#include <fitsio.h>

namespace Pulsar
{
  class FITSHdrExtension;
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
  class DigitiserCounts;
  class FITSSUBHdrExtension;
  class ProfileColumn;
  class CoherentDedispersion;
  class SpectralKurtosis;
  class ObsDescription;
  class CovarianceMatrix;

  class Pointing;
  class AuxColdPlasmaMeasures;

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

    //! Copy all of the class attributes and the selected Integration data
    void copy (const Archive& archive);

    // //////////////////////////////////////////////////////////////////
    //
    // implement the pure virtual methods of the Archive base class
    //

    //! Return a pointer to a new copy constructed instance equal to this
    FITSArchive* clone () const;

    // //////////////////////////////////////////////////////////////////
    //
    // FITSArchive specific interface
    //

    //! Add a new row to the history, reflecting the current state
    void update_history ();

    //! Return the name of the PSRFITS definition template file
    static std::string get_template_name ();

    //! Unload FITSHdrExtension to the current HDU of the specified FITS file
    static void unload (fitsfile* fptr, const FITSHdrExtension*);

    //! Unload ObsExtension to the current HDU of the specified FITS file
    static void unload (fitsfile* fptr, const ObsExtension*);
    
    //! Unload Receiver to the current HDU of the specified FITS file
    static void unload (fitsfile* fptr, const Receiver*);

    //! Unload WidebandCorrelator to the current HDU of the specified FITS file
    static void unload (fitsfile* fptr, const WidebandCorrelator*);

    //! Unload ITRFExtension to the current HDU of the specified FITS file
    static void unload (fitsfile* fptr, const ITRFExtension*);

    //! Unload CalInfoExtension to the current HDU of the specified FITS file
    static void unload (fitsfile* fptr, const CalInfoExtension*);
    
    //! Unload ProcHistory to the HISTORY HDU
    static void unload (fitsfile* fptr, const ProcHistory*);
    
    //! Unload Passband to the BANDPASS HDU
    static void unload (fitsfile* fptr, const Passband*);
    
    //! Unload DigitiserStatistics to the DIG_STAT HDU
    static void unload (fitsfile* fptr, const DigitiserStatistics*);
    
    //! Unload DigitiserCounts to DIG_CNTS HDU
    static void unload (fitsfile* fptr, const DigitiserCounts*);
    
    //! Unload PolnCalibratorExtension to the FEEDPAR HDU
    static void unload (fitsfile* fptr, const PolnCalibratorExtension*);
    
    //! Unload FluxCalibratorExtension to the FLUX_CAL HDU
    static void unload (fitsfile* fptr, const FluxCalibratorExtension*);

    //! Unload CalibratorStokes to the CAL_POLN HDU
    static void unload (fitsfile* fptr, const CalibratorStokes*);
    
    //! Unload FITSSUBHdrExtension
    static void unload (fitsfile *fptr, const FITSSUBHdrExtension*);
    
    //! Unload CoherentDedispersion
    static void unload (fitsfile* fptr, const CoherentDedispersion*);

    //! Unload Pointing to the specified row of the subint table
    static void unload (fitsfile* fptr, const Pointing*, int row);

    //! Unload AuxColdPlasmaMeausres to the specified row of the subint table
    static void unload (fitsfile* fptr, const AuxColdPlasmaMeasures*, int row);

    //! Unload the observation description
    static void unload (fitsfile* fptr, const ObsDescription*);

    //! Unload the Covariance Matrix Data
    static void unload (fitsfile* fptr, const CovarianceMatrix*);

    //! Get the offs_sub value (only present in fits files)
    double get_offs_sub( unsigned int isub ) const;

    // Correct error due to REF_MJD precision
    void refmjd_rounding_correction();

  protected:

    class SKLoader;
    
    friend class Archive::Advocate<FITSArchive>;
    
    // Advocates the use of the FITSArchive plugin
    class Agent : public Archive::Advocate<FITSArchive> {
      
    public:
      
      //! Default constructor (necessary even when empty)
      Agent () {}
      
      //! Advocate the use of FITSArchive to interpret filename
      bool advocate (const char* filename);
      
      //! Return the name of the FITSArchive plugin
      std::string get_name () { return "PSRFITS"; }
      
      //! Return description of this plugin
      std::string get_description ();
      
    };
    
    //! Load the FITS header information from filename
    virtual void load_header (const char* filename);
    
    //! Load the specified Integration from filename, returning new instance
    virtual Integration*
    load_Integration (const char* filename, unsigned subint);
    
    //! The unload_file method is implemented
    bool can_unload () const { return true; }

    //! Unload the FITSArchive (header and Integration data) to filename
    virtual void unload_file (const char* filename) const;

    //! Load data of any type
    template<typename T>
    void load_amps (fitsfile*, Integration*, unsigned isubint, int colnum);

    // //////////////////////////////////////////////////////////////////////
    //
    // ADDITIONAL ATTRIBUTES
    //

    //! The PSRFITS version
    float psrfits_version;

    //! Channel bandwidth
    double chanbw;
    
    //! Double cross coherence term
    bool scale_cross_products;

    //! String describing scale
    std::string state_scale;

    //! String describing polarization state
    std::string state_pol_type;

    //! Extra polyco information stored in POLYCO HDU
    double predicted_phase;

    //! The polyco parsed from the PSRFITS file
    Reference::To<Predictor> hdr_model;
    

    // //////////////////////////////////////////////////////////////////////

    // load the Pulsar::Parameters
    void load_Parameters (fitsfile*);

    // unload the Pulsar::Parameters
    void unload_Parameters (fitsfile*) const;

    // load the Pulsar::Predictor model
    void load_Predictor (fitsfile*);

    // unload the Pulsar::Predictor model
    void unload_Predictor (fitsfile*) const;

    // Archive Extensions used by FITSArchive
    
    // Extension I/O routines
    void load_ProcHistory (fitsfile*);
    void no_ProcHistory (fitsfile*);

    void load_DigitiserStatistics (fitsfile*);
    void load_DigitiserCounts(fitsfile*);
    void load_Passband (fitsfile*);
    void load_PolnCalibratorExtension (fitsfile*);
    void load_FluxCalibratorExtension (fitsfile*);
    void load_CalibratorStokes (fitsfile*);
    void load_Receiver (fitsfile*);
    void load_ITRFExtension (fitsfile*);
    void load_CalInfoExtension (fitsfile*);
    void load_WidebandCorrelator (fitsfile*);
    void load_FITSSUBHdrExtension ( fitsfile * );
    void load_CoherentDedispersion (fitsfile*);
    void load_ObsDescription (fitsfile*);
    void load_CovarianceMatrix (fitsfile*);

    template<class Ext>
    void unload (fitsfile*, const char* hdu_name) const;
    
    void load_integration_state ( fitsfile * );
    void load_state ( fitsfile * );

    void load_Pointing (fitsfile* fptr, int row, Integration*);
    void load_Plasma (fitsfile* fptr, int row, Integration*);
    void load_SpectralKurtosis (fitsfile* fptr, int row, Integration*);

    void interpret_scale ( );
    void interpret_pol_type ( );

    //! Delete the HDU with the specified name
    void delete_hdu (fitsfile* fptr, const char* hdu_name) const;

    // //////////////////////////////////////////////////////////////////////

    // Helper function to write an integration to a file
    void unload_Integration (fitsfile*, int row, const Integration*) const;

    //! Unload Integration data to the SUBINT HDU of the specified FITS file
    void unload_integrations (fitsfile*) const;

    //! Delete Pointing related columns, if not needed
    void clean_Pointing_columns (fitsfile*) const;

    //! Unload Spectral Kurtosis Integration data to the SPECKURT HDU 
    void unload_sk_integrations (fitsfile*) const;

  private:

    // Correct the reference epoch in WBC data taken during commissioning
    void P236_reference_epoch_correction ();

    // Correct the reference epoch in WBC data taken during commissioning
    bool correct_P236_reference_epoch;

    // The data file contains search mode data, not folded pulse profiles
    bool search_mode;

    // Flag set when data are loaded from a PSRFITS file
    bool loaded_from_fits;

    //! Number of auxiliary profiles stored in each channel
    mutable unsigned naux_profile;

    // Reference epoch is used during unload_Integration
    /* This attribute enables proper handling of time stamps when there
       is no FITSHdrExtension in use (as is the case in psrconv) */
    mutable MJD reference_epoch;

    // Profile load/unload algorithm
    mutable Reference::To<ProfileColumn> load_dat_io, unload_dat_io;

    // Prepare dat_io attribute for use
    void setup_dat (fitsfile*, Reference::To<ProfileColumn>&) const;

    // Auxilliary data load/unload algorithm
    mutable Reference::To<ProfileColumn> load_aux_io, unload_aux_io;

    // Prepare dat_io attribute for use
    void setup_aux (fitsfile*, Reference::To<ProfileColumn>&, unsigned) const;

    // Set all attributes to default values
    void init ();

    // Pointer to file used for reading (only)
    fitsfile* read_fptr;

    // Name of file used for reading 
    std::string read_filename;
  };

}

#endif
