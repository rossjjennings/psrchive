//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Database.h,v $
   $Revision: 1.4 $
   $Date: 2004/10/11 14:26:27 $
   $Author: straten $ */

#ifndef __Pulsar_Database_h
#define __Pulsar_Database_h

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

  //! Pulsar Observation Database
  class Database {

  public:

    //! Verbosity flag
    static bool verbose;

    //! Time scale over which calibrator flux and cross-coupling remain stable
    static double long_time_scale;

    //! Time scale over which differential gain and phase remain stable
    static double short_time_scale;

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
    
    //! Return a pointer to a new PolnCalibrator for the given archive
    PolnCalibrator* generatePolnCalibrator (Archive*, Calibrator::Type m);
 
    //! Return a pointer to a new FluxCalibrator for the given archive
    FluxCalibrator* generateFluxCalibrator (Archive*, bool allow_raw=false);
    
    //! Return a pointer to a new HybridCalibrator
    HybridCalibrator* generateHybridCalibrator (ReferenceCalibrator*,Archive*);

    //! Returns the full path to the database summary file
    string get_path () const;
    
    //! Returns the number of entries in the database
    unsigned size () const { return entries.size(); }
 
    //! Pulsar Database Entry
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
      
      static bool match_verbose;

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

    //! Returns a vector of Entry objects that match the given parameters.
    vector<Entry> all_matching (const Criterion& criterion) const;
    
    //! Returns the nearest (in time) Entry that matches the Criterion
    Entry closest_match (const Criterion& criterion,
			 const vector<Entry>& entries);

    //! Return a match using the default_criterion
    Entry PolnCal_match (Pulsar::Archive* arch,
			 Calibrator::Type calType,
			 bool only_observations = false);
    
    //! Return a match using the default_criterion
    Entry allsky_match (Pulsar::Archive* arch,
			Calibrator::Type calType,
			double minutes_apart);
     
    
    //! Returns the full pathname of the Entry filename
    string get_filename (const Entry& entry) const;
    

  protected:
    
    vector<Entry> entries;   // list of entries in the database
    string path;
    
    //! Return a pointer to a new FluxCalibrator for the given archive
    FluxCalibrator* rawFluxCalibrator (Archive* a);

  
  };


}

#endif
