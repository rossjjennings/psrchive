//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Database.h,v $
   $Revision: 1.1 $
   $Date: 2004/07/21 04:03:39 $
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
    
    // Functions for activation and de-activation of tests
    // for various criteria when matching CALs to observations
    
    void test_posn (bool);
    void test_time (bool);
    void test_obst (bool);
    void test_bw (bool);
    void test_freq (bool);
    void test_inst (bool);
    

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
      
      bool check_freq;
      bool check_time;
      bool check_obst;
      bool check_posn;
      bool check_inst;
      bool check_bw;
      
      Criterion ();
      
      //! Return true if entry matches within the criterion
      bool match (const Entry& entry) const;
      
    };
    
    
    //! Return the default matching criterion
    Criterion get_default_criterion () const;
    
    //! Returns a vector of Entry objects that match the given parameters.
    vector<Entry> all_matching (const Criterion& criterion) const;
    
    //! Returns a vector of Entry objects that match the given archive
    vector<Entry> all_matching (Archive* arch, Signal::Source obs_type,
				double minutes_apart, 
				double RA_deg_apart,
				double DEC_deg_apart);
    
    //! All sky search for matches; useful for finding Calibrators
    vector<Entry> all_matching (Pulsar::Archive* arch, const MJD& epoch, 
				Signal::Source obs_type,
				double minutes_apart);
    
    Entry PolnCalibrator_match (Pulsar::Archive* arch,
				Calibrator::Type calType,
				double minutes_apart,
				double RA_deg_apart, 
				double DEC_deg_apart);
    
    //! Returns the nearest (in time) Entry that matches the given parameters
    Entry single_match (Archive* arch, Signal::Source obs_type,
			double minutes_apart, 
			double RA_deg_apart,
			double DEC_deg_apart);
    
    //! Returns the nearest (in time) Entry with the given Calibrator Type
    Entry single_match (Pulsar::Archive* arch, Calibrator::Type calType);
    
    //! Returns the nearest (in time) Entry that matches the Criterion
    Entry closest_match (const Criterion& criterion,
			 const vector<Entry>& entries);
    
    //! Return a pointer to a new HybridCalibrator
    HybridCalibrator* 
    generateHybridCalibrator (ReferenceCalibrator* polcal, Archive* arch);
    
    //! Returns the full pathname of the Entry filename
    string get_filename (const Entry& entry) const;
    

  protected:
    
    vector<Entry> entries;   // list of entries in the database
    string path;
    
    bool check_posn;
    bool check_time;
    bool check_obst;
    bool check_bw;
    bool check_freq;
    bool check_inst;
    
  };


}

#endif
