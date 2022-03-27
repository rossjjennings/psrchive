//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/Database.h

#ifndef __Pulsar_Database_h
#define __Pulsar_Database_h

#include "Pulsar/Calibrator.h"
#include "Pulsar/Config.h"

#include "MEAL/Complex2.h"

#include "sky_coord.h"
#include "MJD.h"
#include "Types.h"

#include <iostream>

namespace Pulsar {

  // forward declarations
  class FluxCalibrator;
  class PolnCalibrator;
  class HybridCalibrator;
  class ReferenceCalibrator;
  class Archive;

  //! Pulsar Observation Database
  class Database : public Reference::Able {

  public:

    //! Cache the last calibrator?
    static bool cache_last_cal;

    //! Print verbose matching information
    static bool match_verbose;

    //! Time scale over which calibrator flux and cross-coupling remain stable
    static Option<double> long_time_scale;

    //! Time scale over which differential gain and phase remain stable
    static Option<double> short_time_scale;

    //! Maximum angular separation between calibrator and pulsar
    static Option<double> max_angular_separation;

    //! Maximum difference between calibrator and pulsar centre frequencies
    static Option<double> max_centre_frequency_difference;

    //! Maximum difference between calibrator and pulsar bandwidths
    static Option<double> max_bandwidth_difference;

    //! Pass this to the criteria methods to retrieve any or all matches
    static const Pulsar::Archive* any;

    //! Calibrator matching sequence
    /*! Different level-setting strategies may dictate the policy
      for matching an observation with the right calibrator */
    enum Sequence {
      //! Use the nearest calibrator (default)
      Any,
      //! Use only calibrators recorded before the observation
      CalibratorBefore,
      //! Use only calibrators recorded after the observation
      CalibratorAfter
    };

    //! Null constructor
    Database ();
    
    //! Construct a database from archives in a metafile
    Database (const std::string& path, const std::string& metafile);

    //! Construct a database from archives in a directory
    Database (const std::string& path, const std::vector<std::string>& exts);

    //! Construct a database from a pre-built ascii file
    Database (const std::string& filename);
    
    //! Destructor.
    ~Database ();

    //! Merge with another database
    void merge (const Database*);

    //! Construct from the list of filenames
    void construct (const std::vector<std::string>& filenames);

    //! Write a text file representing the database to disk for storage.
    void unload (const std::string& dbase_filename);

    //! Read a text file summary and construct a database
    void load (const std::string& dbase_filename);
    
    //! Add the given Archive to the database
    void add (const Pulsar::Archive* archive);

    //! Return a pointer to a new PolnCalibrator for the given archive
    PolnCalibrator* generatePolnCalibrator (Archive*,
					    const Calibrator::Type* m);
 
    //! Return a pointer to a new FluxCalibrator for the given archive
    FluxCalibrator* generateFluxCalibrator (Archive*, bool allow_raw=false);
    
    //! Return a pointer to a new HybridCalibrator
    HybridCalibrator* generateHybridCalibrator (ReferenceCalibrator*,Archive*);

    //! Set the feed transformation to be incorporated into PolnCalibrator
    void set_feed (MEAL::Complex2* xform) { feed = xform; }

    //! Returns the full path to the database summary file
    std::string get_path () const;
    
    //! Returns the number of entries in the database
    unsigned size () const { return entries.size(); }
 
    //! Pulsar Database Entry
    class Entry : public Reference::Able
    {
      
    public:
      
      Reference::To<const Calibrator::Type> calType;

      Signal::Source   obsType;      // FluxCal, PolnCal, Pulsar, etc.
      sky_coord        position;     // Where the telescope was pointing
      double           bandwidth;    // Bandwidth of observation
      double           frequency;    // Centre frequency of observation

      std::string      instrument;   // name of backend
      std::string      receiver;     // name of receiver
      std::string      filename;     // full path of file
      
      //! Construct from a Pulsar::Archive
      Entry (const Archive* arch);

      //! Destructor
      ~Entry();
      
      // factory constructs new Entry from ascii string
      static Entry* load (const std::string& str);

      // factory constructs new Entry from Archive
      static Entry* create (const Archive*);

      // unload ascii string
      virtual void unload (std::string& str);

      // return full path to filename
      std::string get_filename () const;

      virtual bool less_than (const Entry*) const;
      virtual bool equals (const Entry*) const;

      virtual MJD get_effective_time () const = 0;
      virtual std::string get_time_str () const = 0;
      virtual std::string get_nchan_str () const = 0;

    protected:
      
      //! Clean slate
      void init ();
    };

    class StaticEntry : public Entry
    {
    public:
      MJD      time;         // Mid time of observation
      unsigned nchan;        // Number of channels across bandwidth

      //! Construct from a Pulsar::Archive
      StaticEntry (const Archive* arch = 0);

      bool equals (const Entry*) const;

      MJD get_effective_time () const { return time; }
      std::string get_time_str () const;
      std::string get_nchan_str () const;

    protected:
      
      //! Clean slate
      void init ();
    };

    class InterpolatorEntry : public Entry
    {
    public:
      MJD  start_time;   // earliest time spanned by interpolator
      MJD  end_time;     // latest time spanned by interpolator

      //! Construct from a Pulsar::Archive
      InterpolatorEntry (const Archive* arch = 0);

      bool equals (const Entry*) const;

      MJD get_effective_time () const { return start_time; }
      std::string get_time_str () const;
      std::string get_nchan_str () const;
    };
    
    //! Describes Database matching criteria
    class Criteria : public Reference::Able {
      
    public:
      
      //! The parameters to match
      Reference::To<StaticEntry> entry;

      //! The sequence of matching calibrator and pulsar observations
      void set_sequence (Sequence s) { sequence = s; }
      Sequence get_sequence () const { return sequence; }

      double minutes_apart;
      double deg_apart;
      
      bool check_receiver;
      bool check_instrument;
      bool check_frequency;
      bool check_bandwidth;
      bool check_obs_type;
      bool check_time;
      bool check_coordinates;
      bool check_frequency_array;

      //! Default constructor
      Criteria ();
      
      //! Called when no observation parameters are to be matched
      void no_data ();

      //! Return true if entry matches within the criteria
      bool match (const Entry* entry) const;
      
      /** @name match results
       *  
       * These attributes are set by the match method
       */
      //@{

      //! log of the attributes compared and the result of the comparison
      mutable std::string match_report;

      //! count of the number of match conditions that tested positive
      mutable unsigned match_count;

      //! time between calibrator and requested epoch (in minutes)
      mutable double diff_minutes;

      //! distance between calibrator and requested position (in degrees)
      mutable double diff_degrees;

      //@}

      //! Return the best of two entries
      const Entry* best (const Entry* a, const Entry* b) const;

      //! Return the criteria that came closest to matching
      static Criteria closest (const Criteria& a, const Criteria& b);

    protected:

      //! The sequence of matching calibrator and pulsar observations
      Sequence sequence;

      template<typename T, typename U, typename Predicate>
      void compare (const std::string& name,
		    const T& want, const U& have,
		    Predicate equal ) const
      {
	match_report += "\t" + name
	  + " want=" + tostring(want) + " have=" + tostring(have) + " ... ";
	
	if ( equal (want, have) )
	{
	  match_report += "match \n";
	  match_count ++;
	}
	else
	{
	  match_report += "no match \n";
	  throw false;
	}
      }

      template<typename T>
      void compare (const std::string& name,
		    const T& want, const T& have) const
      {
	compare (name, want, have, std::equal_to<T>());
      }

      bool compare_times (const MJD& want, 
			  const MJD& have) const;

      bool bracketed_time (const MJD& want,
			   const std::pair<MJD,MJD>& have) const;

      bool compare_coordinates (const sky_coord& want,
				const sky_coord& have) const;
    };
    
    
    //! Get the default matching criteria for all observations
    static Criteria get_default_criteria ();

    //! Set the default matching criteria for all observations
    static void set_default_criteria (const Criteria&);

    //! Returns the best Entry that matches the given Criteria
    const Entry* best_match (const Criteria&) const;

    //! Fills a vector with Entry instances that match the given Criteria
    void all_matching (const Criteria&,
		       std::vector<const Entry*>& matches) const;
    
    //! Return the Criteria for the specified Pulsar::Archive
    Criteria criteria (const Pulsar::Archive* arch,
		       Signal::Source obsType) const;
    
    //! Return the Criteria for the specified Pulsar::Archive
    Criteria criteria (const Pulsar::Archive* archive,
		       const Calibrator::Type* calType) const;

    //! Returns the full pathname of the Entry filename
    std::string get_filename (const Entry*) const;
    
    //! Add the given entry to the database
    void add (Entry* entry);

    //! Remove the preceding path from the Entry filename, if applicable
    void shorten_filename (Entry* entry);

    //! Add path to Entry filename, if applicable
    void expand_filename (Entry* entry);

    //! Get the closest match report
    std::string get_closest_match_report () const;

  protected:

    // list of entries in the database
    std::vector< Reference::To<Entry> > entries;
    std::string path;

    template<class Cal> class Cache
    {
    public:
      Reference::To<const Entry> entry;
      Reference::To<Cal> calibrator;

      void cache (const Entry* _entry, Cal* cal)
      { entry = _entry; calibrator = cal; }
    };

    Cache<PolnCalibrator> lastPolnCal;
    Cache<HybridCalibrator> lastHybridCal;
    Cache<FluxCalibrator> lastFluxCal;
 
    //! Return a pointer to a new FluxCalibrator for the given archive
    FluxCalibrator* rawFluxCalibrator (Archive* a);

    //! If set, this model of the feed is incorporated into all solutions
    Reference::To<MEAL::Complex2> feed;

    //! The criteria that last came closest to matching
    mutable Criteria closest_match;
  };

  std::ostream& operator << (std::ostream& os, Database::Sequence);
    
  std::istream& operator >> (std::istream& is, Database::Sequence&);
}

#endif
