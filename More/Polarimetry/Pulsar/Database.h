//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Database.h,v $
   $Revision: 1.2 $
   $Date: 2004/07/21 05:27:41 $
   $Author: straten $ */

#ifndef __Pulsar_Calibration_Database_h
#define __Pulsar_Calibration_Database_h

#include "Pulsar/Calibrator.h"

#include "ReferenceAble.h"
#include "sky_coord.h"
#include "MJD.h"
#include "Types.h"

namespace Pulsar {

  // forward declarations
  class FluxCalibrator;
  class PolnCalibrator;
  class HybridCalibrator;
  class ReferenceCalibrator;
  class Archive;

  //! Pulsar Calibrator Observation Database
  class Database {

  public:

    //! Verbosity flag
    static bool verbose;

    //! Null constructor
    Database ();
    
    //! Construct a database from archives in a directory
    Database (string path, const vector<string>& extensions);
    
    //! Construct a database from a pre-built ascii file
    Database (const char* filename);
    
    //! Destructor.
    ~Database ();
    
    //! Write a text file representing the database to disk for storage.
    void unload (const char* dbase_filename);

    //! Read a text file summary and construct a database
    void load (const char* dbase_filename);
    
    //! Return a pointer to a new FluxCalibrator for the given archive
    FluxCalibrator* generateFluxCalibrator (Archive* a);
    
    //! Return a pointer to a new PolnCalibrator for the given archive
    PolnCalibrator* generatePolnCalibrator (Archive* a, Calibrator::Type m);
    
    //! Returns the full path to the database summary file
    string get_path () const;
    
    //! Returns the number of entries in the database
    unsigned size () const { return entries.size(); }
 
    //! Pulsar Calibration Database Entry
    class Entry {
      
    public:
      
      // Critical information about the entry
      
      Signal::Source   obsType;      // FluxCal, PolnCal, Pulsar, etc.
      Calibrator::Type calType;      // SingleAxis, Britton, etc.
      MJD              time;         // Mid time of observation
      sky_coord        position;     // Where the telescope was pointing
      double           bandwidth;    // Bandwidth of observation
      double           frequency;    // Centre frequency of observation
      unsigned         nchan;        // Number of channels across bandwidth
      string           instrument;   // name of backend
      string           filename;     // relative path of file
      
      //! Null constructor
      Entry () { init(); }
      //! Construct from an ASCII string
      explicit Entry (const char* txt) { load(txt); }
      explicit Entry (string& str) { load(str); }
      //! Construct from a Pulsar::Archive
      Entry (const Archive& arch);
      //! Destructor
      ~Entry();
      
      // load from ascii string
      void load (const char* str);
      void load (string& str) { load(str.c_str()); }
      // unload ascii string
      void unload (string& str);
      
    protected:
      
      //! Clean slate
      void init ();
      
    };
    
    class Criterion  {
      
    public:
      
      //! The parameters to match
      Entry entry;
      
      double minutes_apart;
      
      double RA_deg_apart;
      
      double DEC_deg_apart;
      
      bool check_instrument;
      bool check_frequency;
      bool check_bandwidth;
      bool check_obs_type;
      bool check_time;
      bool check_coordinates;
     
      Criterion ();
      
      //! Return true if entry matches within the criterion
      bool match (const Entry& entry) const;
      
    };
    
    
    //! Get the default matching criterion for all observations
    static Criterion get_default_criterion ();

    //! Set the default matching criterion for all observations
    static void set_default_criterion (const Criterion& criterion);

    //! Get the default matching criterion for PolnCal observations
    static Criterion get_default_PolnCal_criterion ();

    //! Get the default matching criterion for FluxCal observations
    static Criterion get_default_FluxCal_criterion ();

    //! Get the default matching criterion for Reception model solutions
    static Criterion get_default_Reception_criterion ();

    //! Get the matching criterion for PolnCal observations
    Criterion get_PolnCal_criterion () const;

    //! Get the matching criterion for FluxCal observations
    Criterion get_FluxCal_criterion () const;

    //! Get the matching criterion for Reception model solutions
    Criterion get_Reception_criterion () const;

    //! Set the matching criterion for PolnCal observations
    void set_PolnCal_criterion (const Criterion& criterion) const;

    //! Set the matching criterion for FluxCal observations
    void set_FluxCal_criterion (const Criterion& criterion) const;

    //! Set the matching criterion for Reception model solutions
    void set_Reception_criterion (const Criterion& criterion) const;

    //! Returns a vector of Entry objects that match the given parameters.
    vector<Entry> all_matching (const Criterion& criterion) const;
    
    //! Returns the nearest (in time) Entry that matches the Criterion
    Entry closest_match (const Criterion& criterion,
			 const vector<Entry>& entries);

    //! Return a match using the default_PolnCal_criterion
    Entry PolnCal_match (Pulsar::Archive* arch,
			 Calibrator::Type calType,
			 bool only_observations = false);
    
    //! Return a match using the default_Reception_criterion
    Entry Reception_match (Pulsar::Archive* arch,
			   Calibrator::Type calType);
     
    //! Return a pointer to a new HybridCalibrator
    HybridCalibrator* 
    generateHybridCalibrator (ReferenceCalibrator* polcal, Archive* arch);
    
    //! Returns the full pathname of the Entry filename
    string get_filename (const Entry& entry) const;
    

  protected:
    
    vector<Entry> entries;   // list of entries in the database
    string path;
    
    
  };


}

#endif
