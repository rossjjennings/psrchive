//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Archive.h,v $
   $Revision: 1.112 $
   $Date: 2004/06/17 09:15:51 $
   $Author: straten $ */

/*! \mainpage 
 
  \section intro Introduction
 
  The Pulsar Data Archival and Analysis Library implements a set of
  base classes (in C++) that may be used in the storage, manipulation,
  and analysis of the observational data used in pulsar experiments.
  The base classes implement a minimal set of general, flexible
  routines.
 
  \section profiles Pulse Profiles
 
  The basic quantity observed in most pulsar experiments is the pulse
  profile, a one-dimensional array of some measured quantity as a
  function of pulse phase.  This is represented by the Pulsar::Profile
  class.  The Pulsar::Integration class contains a two-dimensional
  array of Pulsar::Profile objects, each integrated over the same time
  interval.  The Pulsar::Profile objects are usually organized as a
  function of polarimetric measure and observing frequency, though
  other measured states are possible.  The Pulsar::Archive class
  implements a one-dimensional array of Pulsar::Integration objects,
  each with similar observational parameters and each in "pulse phase"
  with eachother.

  \section minimal Minimal Interface

  The Pulsar::Profile class implements a minimal set of operations
  required to manipulate a pulsar profiles in the phase domain.  These
  include, but are not limited to:
  <UL>
  <LI> operator += - adds offset to each bin of the profile </LI>
  <LI> operator *= - multiplies each bin of the profile by scale </LI>
  <LI> rotate - rotates the profile in phase </LI>
  <LI> bscrunch - integrates neighbouring phase bins in profile </LI>
  <LI> fold - integrates neighbouring sections of the profile </LI>
  </UL>

  As well, a basic set of routines are included that may be used to
  calculate statistics, find minima and maxima, find the shift between
  the profile and a standard, etc..  Combinations of these functions
  can be used to perform basic tasks.  For instance, baseline removal
  is simply and transparently implemented as:

  <pre> profile -= mean (find_min_phase()); </pre> 

  The Pulsar::Integration class implements a minimal set of operations
  required to manipulate a set of Pulsar::Profile objects in the
  polarization and frequency domains.  In addition to simple nested
  calls of the above functions, these include:
  <UL>
  <LI> dedisperse - rotates all profiles to remove dispersion delays 
  between channels </LI>
  <LI> defaraday - transforms all profiles to remove faraday rotation
  between channels </LI>
  <LI> fscrunch - integrates profiles from neighbouring channels </LI>
  <LI> pscrunch - integrates profiles from two polarizations into 
  one total intensity </LI>
  <LI> invint - forms the polarimetric invariant interval from 
  Stokes (I,Q,U,V) </LI>
  <LI> transform - performs a polarimetric transformation </LI>
  </UL>

  The Pulsar::Archive virtual base class is the interface that will be
  used in most applications.  In addition to providing interfaces to
  all of the above functions, the Pulsar::Archive class implements:
  <UL>
  <LI> tscrunch - integrates profiles from neighbouring Integrations </LI>
  <LI> append - copies (or transfers) the Integrations from one Archive
  to another </LI>
  <LI> set_ephemeris - installs a new ephemeris and polyco </LI>
  <LI> set_polyco - installs a new polynomial and aligns all profiles
  to it </LI>
  </UL>

  For a complete list of the methods defined in each of these base classes,
  please see the <a href="annotated.html">Compound List</a>.

  \section format Archive File Formats

  The Pulsar::Archive virtual base class specifies only the minimal
  set of information required in order to implement basic data
  reduction algorithms.  Although it specifies the interface to set
  and get the values of various attributes and load and unload data
  from file, no storage or I/O capability is implemented by
  Pulsar::Archive.  These methods, especially those related to file
  I/O, must be implemented by classes that inherit the Pulsar::Archive
  base class.

  Most observatories and research groups use unique file formats and
  associate different pieces of information with their observations.
  The derived classes must therefore implement the storage and
  modification of this auxilliary information.  For this reason, most
  of the Pulsar::Archive methods are declared virtual, and may be
  over-ridden as in the following example:

  <pre>
  class B : public Pulsar::Archive {
    public:
      void append (Pulsar::Archive* aptr);
  };

  void B::append (Pulsar::Archive* aptr)
  {
    // call the function implemented by Pulsar::Archive
    Pulsar::archive::append (aptr);

    // dynamic_cast returns a pointer only if aptr points to an instance of B
    B* bptr = dynamic_cast<B*>(aptr);

    // test if *aptr is a B
    if (!bptr)
      return;

    // do B-specific things
  }
  </pre>

  In general, applications need not know anything about the specific
  archive file format with which they are dealing.  New Pulsar::Archive
  instances may be created and loaded from file by calling the static
  Pulsar::Archive::load factory.  For instance:

  <pre>
  Pulsar::Archive* archive = Pulsar::Archive::load (filename);
  </pre>

  \subsection plugin File Format Plugins

  Classes that inherit Pulsar::Archive and implement the I/O routines
  for a specific archive file format are loaded as plugins.  Plugins
  are registered for use in applications by inheriting the
  Pulsar::Archive::Advocate template base class.  The Advocate class
  specifies three pure virtual methods that must be implemented by a
  sub-class of the Archive-derived class named "Agent".  For example:

  <pre>
  class ArchiveFormat : public Pulsar::Archive {

    [...]

    class Agent : public Pulsar::Archive::Advocate<ArchiveFormat>  {

      //! Advocate the use of the derived class to interpret filename
      virtual bool advocate (const char* filename);
      
      //! Return the name of the derived class
      virtual string get_name () { return "ArchiveFormat"; }

      //! Return a description of the derived class
      virtual string get_description () { return "Version 0.1"; }

    };

  };

  </pre>

 */

#ifndef __Pulsar_Archive_h
#define __Pulsar_Archive_h

#include <vector>
#include <string>

#include <stdio.h>

#include "IntegrationManager.h"
#include "polyco.h"
#include "psrephem.h"
#include "sky_coord.h"

#include "Registry.h"
#include "Types.h"

#include "toa.h"

template<typename T> class Jones;

//! The root level namespace, containing everything pulsar related
namespace Pulsar {

  class Integration;
  class Profile;

  //! Group of Pulsar::Integration observations with the same pulse phase.
  /*! This pure virtual base class implements the storage and manipulation
    of a vector of Pulsar::Integration objects.  Each Pulsar::Integration
    has similar attributes, such as centre frequency, bandwidth, and source,
    and each contains profiles that are aligned to start on the same pulse
    phase. */

  class Archive : public IntegrationManager {

  private:

    //! Pure virtual base class of Archive::Advocate template base class
    /*! This class is inherited through the Archive::Advocate template. */
    class Agent : public Reference::Able {

    public:

      //! Destructor
      virtual ~Agent ();

      //! Advocate the use of the derived class to interpret filename
      virtual bool advocate (const char* filename) = 0;
      
      //! Return the name of the derived class
      virtual string get_name () = 0;

      //! Return a description of the derived class
      virtual string get_description () = 0;

      //! Return the revision of the Archive base class definition
      virtual string get_revision () = 0;

      //! Return a null-constructed instance of the derived class
      virtual Archive* new_Archive () = 0;

      //! Return the name of the plugins directory
      static string get_plugin_path (const char* shell_variable = "CVSHOME");

      //! Report to cerr on the status of the Registry (and plugins)
      static void report ();

    protected:

      //! Agents registered for creating derived classes in Archive::load
      static Registry::List<Agent> registry;
      
      //! Declare friends with Registry::Entry<Agent> so it can access registry
      friend class Registry::Entry<Agent>;

      //! The path from which plugin code will be loaded
      static string plugin_path;

      //! Declare friends with Archive so Archive::load can access registry
      friend class Archive;

      //! Flag that plugin_load has been called
      static bool loaded;

      //! Initialization function calls plugin_load if dynamic linkage enabled
      static bool init ();

      //! Load plugins from the plugin_path
      static void plugin_load ();

      //! Report to cerr on the status of the plugins
      static void plugin_report ();

      //! Verify that the plugin revision strings match those of the base class
      static void verify_revisions ();

    };

  public:

    //! Return the revision number of the Archive base class definition
    /*! This string is automatically generated by CVS.  Do not edit. */
    static string get_revision () { return get_revision("$Revision: 1.112 $"); }

    //! Parses the revision number out of the CVS Revision string
    static string get_revision (const char* revision);

    //! Classes derived from Archive are registered for use via an Advocate
    /*! This abstract template base class must be inherited in order
      to register plugins for use with the Archive::load factory.  The
      following abstract methods of the Agent base class must be defined:

      //! Advocate the use of the derived class to interpret filename
      virtual bool advocate (const char* filename) = 0;
      
      //! Return the name of the derived class
      virtual string get_name () = 0;

      //! Return a description of the derived class
      virtual string get_description () = 0;

    */
    template<class Type>
    class Advocate : public Agent {

    public:

      //! Constructor ensures that template entry is instantiated
      Advocate () { entry.get(); }

      //! Return a new instance of the Archive derived class
      Archive* new_Archive () { return new Type; }

      //! Return the revision number of the Archive base class definition
      /*! This string is automatically generated by CVS.  Do not edit. */
      string get_revision () 
      {
	return Archive::get_revision ("$Revision: 1.112 $");
      }

    private:

      //! Enter template constructor adds Advocate<Type> to Agent::registry
      static Registry::List<Archive::Agent>::Enter<typename Type::Agent> entry;

    };

    //! Abstract base class of Archive::Extension objects
    /* Archive derived classes may provide access to additional informaton
       through Extension-derived objects. */
    class Extension : public Reference::Able {

    public:

      //! Construct with a name
      Extension (const char* name);

      //! Destructor
      virtual ~Extension ();

      //! Return a new copy-constructed instance identical to this instance
      virtual Extension* clone () const = 0;

      //! Return the name of the Extension
      string get_name () const;

    protected:

      //! Extension name - useful when debugging
      string name;

    };

    //! Public access to Agent::report
    static void agent_report ();

    //! Set Agent::plugin_path
    static void set_plugin_path (const char* path);

    //! Archive::append should enforce chronological order
    static bool append_chronological;

    //! Archive::append should assume that bin zero has polyco phase zero
    static bool append_phase_zero;

    //! Archive::append should throw exception if Archive::match fails
    static bool append_must_match;

    //! Amount by which integration intervals may overlap in Archive::append
    static double append_max_overlap;

    //! Ok to mix upper/lower sideband in Archive::calibrator_match
    static bool match_opposite_sideband;

    //! Maximum centre frequency difference in Archive::calibrator_match
    static double match_max_frequency_difference;

    //! String inserted between match failure reason strings
    static string match_indent;

    //! Weight integrations by their integration length, or duration
    static bool weight_by_duration;

    //! Set the verbosity level (0 to 3)
    static void set_verbosity (unsigned level);

    // //////////////////////////////////////////////////////////////////
    //
    // constructors, destructor, operator =,
    //
    // //////////////////////////////////////////////////////////////////

    //! Default constructor
    Archive ();

    //! Copy constructor
    Archive (const Archive& archive);
    
    //! Destructor
    virtual ~Archive ();

    //! Operator =
    Archive& operator = (const Archive& a);

    //! Copy all of the base class attributes and Integration data
    void copy (const Archive& archive);

    //! Copy all of the base class attributes and the selected Integration data
    virtual void copy (const Archive& archive, const vector<unsigned>& ints);

    //! Return a null-constructed instance of the derived class
    static Archive* new_Archive (const string class_name);


    // //////////////////////////////////////////////////////////////////
    //
    // Extension access
    //
    // //////////////////////////////////////////////////////////////////

    //! Return the number of extensions available
    virtual unsigned get_nextension () const;

    //! Return a pointer to the specified extension
    virtual const Extension* get_extension (unsigned iextension) const;

    //! Return a pointer to the specified extension
    virtual Extension* get_extension (unsigned iextension);

    //! Template method searches for an Extension of the specified type
    template<class ExtensionType>
    const ExtensionType* get () const;

    //! Template method searches for an Extension of the specified type
    template<class ExtensionType>
    ExtensionType* get ();

    //! Template method returns an Extension of the specified type
    template<class ExtensionType>
    ExtensionType* getadd ();

    //! Add an Extension to the Archive instance
    /*! The derived class must ensure that only one instance of the Extension
      type is stored.

      \return On successful addition, this method should return the
      pointer to the Extension, equal to the extension argument.  If the
      Extension is not supported, this method should return a null pointer.
    */
    virtual void add_extension (Extension* extension);

    // //////////////////////////////////////////////////////////////////
    //
    // Data access and resize
    //
    // //////////////////////////////////////////////////////////////////

    //! Return pointer to the specified profile
    Profile* get_Profile (unsigned subint, unsigned pol, unsigned chan);

    //! Return pointer to the specified profile
    const Profile*
    get_Profile (unsigned subint, unsigned pol, unsigned chan) const;

    // get_Integration implemented by the IntegrationManager base class

    //! Returns a block of amplitudes ordered according to the specified axis
    void get_amps (vector<float>& amps,
		   Signal::Dimension x1 = Signal::Phase,
		   Signal::Dimension x2 = Signal::Frequency,
		   Signal::Dimension x3 = Signal::Polarization) const;

    //! Resize the Integration vector with new_Integration instances
    virtual void resize (unsigned nsubint, 
			 unsigned npol=0, unsigned nchan=0, unsigned nbin=0);
    
    // //////////////////////////////////////////////////////////////////
    //
    // File loading and unloading
    //
    // //////////////////////////////////////////////////////////////////

    //! Dynamic constructor loads an Archive subclass from filename
    static Archive* load (const char* filename);

    //! Convenience interface to Archive::load (const char*)
    static Archive* load (const string& name);

    //! Write the archive to filename
    void unload (const char* filename = 0);

    //! Convenience interface to Archive::unload (const char*)
    void unload (const string& filename) { unload (filename.c_str()); }

    //! Get the name of the file to which the archive will be unloaded
    string get_filename () const { return unload_filename; }

    //! Set the filename of the Archive
    /*! The filename is the name of the file to which the archive will be 
      written on the next call to Archive::unload, if no arguments are given
      to the Archive::unload method. */
    void set_filename (const char* filename) { unload_filename = filename; }

    //! Convenience interface to Archive::set_filename (const char*)
    void set_filename (const string& filename) { unload_filename = filename; }

    //! Update the current archive, saving current Integration data
    void update ();

    //! Completely reload the archive, deleting all data
    void refresh ();

    // //////////////////////////////////////////////////////////////////
    //
    // basic algorithms implemented by Archive
    //
    // //////////////////////////////////////////////////////////////////

    //! Integrate pulse profiles in phase
    virtual void bscrunch (unsigned nscrunch);

    //! Integrate neighbouring sections of the pulse profiles
    virtual void fold (unsigned nfold);

    //! Integrate profiles in polarization
    virtual void pscrunch();

    //! Integrate profiles in frequency
    virtual void fscrunch (unsigned nscrunch=0, bool weighted_cfreq = true);

    //! Integrate profiles in time
    virtual void tscrunch (unsigned nscrunch=0);

    //! Rotate each profile by time seconds
    virtual void rotate (double time);

    //! Append the Integrations from 'archive' to 'this'
    virtual void append (const Archive* archive);

    //! Append frequency channels from another Archive
    //
    //  Note that this is only intended for use with instruments
    //  whose band is split into adjoining segments (like CPSR2)
    void fappend (Pulsar::Archive* arch, bool ignore_time_mismatch = false);

    //! Return pointer to a new fscrunched, tscrunched and pscrunched clone
    Archive* total () const;

    //! denoise archive
    virtual void denoise (int denoise_fraction=8) ;
    //! split archive into subbands
    virtual void split (int split_fraction=8);

    //! Rotate pulsar Integrations so that pulse phase zero is centred
    virtual void centre ();

    /*! Rotate pulsar Integrations so that the bin of largest amplitude
        is centred */
    virtual void centre_max_bin (); 

    //! Rotate the Profiles to remove dispersion delays b/w chans
    virtual void dedisperse (double dm = 0.0, double frequency = 0.0);

    //! Correct the Faraday rotation of Q into U
    virtual void defaraday (double rotation_measure = 0);
    
    //! Fit Profiles to the standard and return toas
    virtual void toas (vector<Tempo::toa>& toas, const Archive* std,
		       string arguments = "", bool time_domain = false,
		       Tempo::toa::Format fmt = Tempo::toa::Parkes) const;
    
    //! Correct receiver feed angle orientation
    virtual void deparallactify();

    //! Un-correct receiver feed angle orientation
    virtual void parallactify();

    //! Correct or un-correct parallactic angle rotation
    virtual void parallactic (bool correct);

    //! Convert polarimetric data to the specified state
    virtual void convert_state (Signal::State state);

    //! Perform the transformation on each polarimetric profile
    virtual void transform (const Jones<float>& transformation);

    //! Perform frequency response on each polarimetric profile
    virtual void transform (const vector< Jones<float> >& response);

    //! Perform the time and frequency response on each polarimetric profile
    virtual void transform (const vector< vector< Jones<float> > >& response);

    //! Transform Stokes I,Q,U,V into the polarimetric invariant interval
    virtual void invint ();
 
    //! Return polarization flux in first integration, 0 == total, 1 == linear, 2==circular 
    virtual float get_poln_flux (int _type);
  
    //! Remove the baseline from all profiles
    virtual void remove_baseline (float phase = -1.0, float dc = 0.15);

    //! Install the given ephemeris and call update_model
    virtual void set_ephemeris (const psrephem& ephemeris, bool update = true);

    //! Return a copy of the current archive ephemeris
    const psrephem get_ephemeris();

    //! Install the given polyco and shift profiles to align
    virtual void set_model (const polyco& model);

    //! Return a copy of the current archive polyco
    const polyco get_model();

    //! Create a new polyco and align the Integrations to the new model
    virtual void update_model ();

    //! Set the weight of each profile to its snr squared
    virtual void snr_weight ();

    //! Set the weight of each profile to the given number
    virtual void uniform_weight (float new_weight = 1.0);

    //! Test if arch matches (enough for a pulsar - standard match)
    virtual bool standard_match (const Archive* arch, string& reason) const;

    //! Test if arch matches (enough for a pulsar - calibrator match)
    virtual bool calibrator_match (const Archive* arch, string& reason) const;

    //! Test if arch matches (enough for a pulsar - pulsar match)
    virtual bool processing_match (const Archive* arch, string& reason) const;

    //! Test if arch is mixable (enough for append)
    virtual bool mixable (const Archive* arch, string& reason) const;

    //! Computes the weighted channel frequency over an Integration interval.
    double weighted_frequency (unsigned ichan,
			       unsigned start, unsigned end) const;

    //! Call bscrunch with the appropriate value
    void bscrunch_to_nbin (unsigned new_nbin);

    //! Halve the bins
    void halvebins (unsigned nhalve);

    //! Call fscrunch with the appropriate value
    void fscrunch_to_nchan (unsigned new_nchan);

    //! Call tscrunch with the appropriate value
    void tscrunch_to_nsub (unsigned new_nsub);
    
    //! Use a new folding period
    void new_folding_period( double new_period);

    //! Return the MJD at the start of the first sub-integration
    MJD  start_time () const;

    //! Return the MJD at the end of the last sub-integration
    MJD  end_time () const;

    //! Returns the total time integrated into all Integrations (in seconds)
    double integration_length() const;

    //! Return true if the observation is a calibrator
    bool type_is_cal () const;

    //! Return the coordinates of the telescope at which observation was made
    /*!
      \retval latitude in degrees
      \retval longitude in degrees
      \retval elevation in metres
    */
    void telescope_coordinates (float* latitude = 0,
				float* longitude = 0,
				float* elevation = 0) const;

    //! Find the transitions between high and low states in total intensity
    void find_transitions (int& hi2lo, int& lo2hi, int& buffer) const;

    //! Find the bins in which the total intensity exceeds a threshold
    void find_peak_edges (int& rise, int& fall) const;

    //! Returns the centre phase of the region with maximum total intensity
    float find_max_phase () const;

    //! Returns the centre phase of the region with minimum total intensity
    float find_min_phase (float dc = 0.15) const;

    //! Returns geometric mean of baseline RMS for each Integration
    float rms_baseline (float dc = 0.4);

    //! Returns the best period based on SNR for an archive
    double find_best_period () ;

    //! A dsp::Transformation into an Archive must be able to call this
    //! This calls Signal::valid_state() to see if the state is consistent with the ndim, npol
    virtual bool state_is_valid(string& reason) const;

    // //////////////////////////////////////////////////////////////////
    //
    // pure virtual methods - must be implemented by derived classes
    //
    // //////////////////////////////////////////////////////////////////

    //! Return a pointer to a new copy constructed instance equal to this
    virtual Archive* clone () const = 0;

    //! Return a pointer to a new extraction constructed instance equal to this
    virtual Archive* extract (const vector<unsigned>& subints) const = 0;

    //! Get the number of pulsar phase bins used
    /*! This attribute may be set only through Archive::resize */
    virtual unsigned get_nbin () const = 0;

    //! Get the number of frequency channels used
    /*! This attribute may be set only through Archive::resize */
    virtual unsigned get_nchan () const = 0;

    //! Get the number of frequency channels used
    /*! This attribute may be set only through Archive::resize */
    virtual unsigned get_npol () const = 0;

    //! Get the tempo code of the telescope used
    virtual char get_telescope_code () const = 0;
    //! Set the tempo code of the telescope used
    virtual void set_telescope_code (char telescope_code) = 0;

    //! Get the feed configuration of the receiver
    virtual Signal::Basis get_basis () const = 0;
    //! Set the feed configuration of the receiver
    virtual void set_basis (Signal::Basis type) = 0;

    //! Get the state of the profiles
    virtual Signal::State get_state () const = 0;
    //! Set the state of the profiles
    virtual void set_state (Signal::State state) = 0;

    //! Get the observation type (psr, cal)
    virtual Signal::Source get_type () const = 0;
    //! Set the observation type (psr, cal)
    virtual void set_type (Signal::Source type) = 0;

    //! Get the source name
    virtual string get_source () const = 0;
    //! Set the source name
    virtual void set_source (const string& source) = 0;

    //! Get the backend
    virtual string get_backend () const = 0;
    //! Set the backend
    virtual void set_backend (const string& bak) = 0;

    //! Get the receiver
    virtual string get_receiver () const = 0;
    //! Set the receiver
    virtual void set_receiver (const string& rec) = 0;

    //! Get the coordinates of the source
    virtual sky_coord get_coordinates () const = 0;
    //! Set the coordinates of the source
    virtual void set_coordinates (const sky_coord& coordinates) = 0;

    //! Get the centre frequency of the observation
    virtual double get_centre_frequency () const = 0;
    //! Set the centre frequency of the observation
    virtual void set_centre_frequency (double cf) = 0;

    //! Get the overall bandwidth of the observation
    virtual double get_bandwidth () const = 0;
    //! Set the overall bandwidth of the observation
    virtual void set_bandwidth (double bw) = 0;

    //! Get the dispersion measure (in \f${\rm pc\, cm}^{-3}\f$)
    virtual double get_dispersion_measure () const = 0;
    //! Set the dispersion measure (in \f${\rm pc\, cm}^{-3}\f$)
    virtual void set_dispersion_measure (double dm) = 0;

    //! Inter-channel dispersion delay has been removed
    virtual bool get_dedispersed () const = 0;
    //! Set the value to be returned by get_dedispersed
    virtual void set_dedispersed (bool done = true) = 0;

    //! Get the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
    virtual double get_rotation_measure () const = 0;
    //! Set the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
    virtual void set_rotation_measure (double rm) = 0;

    //! Data has been corrected for ISM faraday rotation
    virtual bool get_ism_rm_corrected () const = 0;
    //! Set the value to be returned by get_ism_rm_corrected
    virtual void set_ism_rm_corrected (bool done = true) = 0;

    //! Data has been corrected for ionospheric faraday rotation
    virtual bool get_iono_rm_corrected () const = 0;
    //! Set the value to be returned by get_iono_rm_corrected
    virtual void set_iono_rm_corrected (bool done = true) = 0;

    //! Data has been corrected for parallactic angle errors
    virtual bool get_parallactic_corrected () const = 0;
    //! Set the value to be returned by get_parallactic_corrected
    virtual void set_parallactic_corrected (bool done = true) = 0;

    //! Data has been corrected for feed angle errors
    virtual bool get_feedangle_corrected () const = 0;
    //! Set the value to be returned by get_feedangle_corrected
    virtual void set_feedangle_corrected (bool done = true) = 0;

    //! Data has been calibrated for polarimetric response of instrument
    virtual bool get_poln_calibrated () const = 0;
    //! Set the value to be returned by get_poln_calibrated
    virtual void set_poln_calibrated (bool done = true) = 0;

    //! Data has been flux calibrated
    virtual bool get_flux_calibrated () const = 0;
    //! Set the value to be returned by get_flux_calibrated
    virtual void set_flux_calibrated (bool done = true) = 0;

  protected:
    
    // Classes that inherit from IntegrationOrder need low-level access
    friend class BinaryPhaseOrder;
    friend class PeriastronOrder;
    friend class BinLngPeriOrder;
    friend class BinLngAscOrder;   
 
    //! Set the number of pulsar phase bins
    /*! Called by Archive methods to update child attribute */
    virtual void set_nbin (unsigned numbins) = 0;

    //! Set the number of frequency channels
    /*! Called by Archive methods to update child attribute */
    virtual void set_nchan (unsigned numchan) = 0;

    //! Set the number of polarization measurements
    /*! Called by Archive methods to update child attribute */
    virtual void set_npol (unsigned numpol) = 0;

    //! Load the header information from filename
    virtual void load_header (const char* filename) = 0;

    //! Load the specified Integration from filename, returning new instance
    virtual Integration*
    load_Integration (const char* filename, unsigned subint) = 0;

    //! Unload the Archive (header and Integration data) to filename
    virtual void unload_file (const char* filename) const = 0;

    // //////////////////////////////////////////////////////////////////
    //
    // end of pure virtual methods 
    //
    // //////////////////////////////////////////////////////////////////

    //! Name of file to which the archive will be written on call to unload()
    string unload_filename;

    //! The pulsar ephemeris, as used by TEMPO
    Reference::To<psrephem> ephemeris;

    //! The pulsar phase model, as created using TEMPO
    Reference::To<polyco> model;

    //! The Extensions added to this Archive instance
    vector< Reference::To<Extension> > extension;
    
    //! Initialize an Integration to reflect Archive attributes.
    void init_Integration (Integration* subint);

    //! Provide Integration::resize access to Archive-derived classes
    void resize_Integration (Integration* integration);
    
    //! Apply the current model to the Integration
    void apply_model (const polyco& old, Integration* subint);

    //! Update the polyco model and correct the Integration set
    void update_model (unsigned old_nsubint);

    //! Update the polyco to include the specifed MJD
    void update_model (const MJD& time, bool clear_model = false);

    //! Creates polynomials to span the Integration set
    void create_updated_model (bool clear_old);

    //! Returns true if the given model spans the Integration set
    bool good_model (const polyco& test_model) const;

    //! Default selected subints
    static const vector<unsigned> none_selected;


  private:

    //! This flag may be raised only by Archive::update_model.
    /*!
      As it is set only during run-time, this flag makes it known that
      the current Integration set has been aligned to a polyco created
      during run-time
    */
    bool model_updated;

    //! Store the name of the file from which the current instance was loaded
    /*! Although the logical name of the file may be changed with
      Archive::set_filename, the base class must keep track of the
      original file in order to read unloaded information from this
      file when it is required. */
    string __load_filename;

    //! Load a new instance of the specified integration from __load_filename
    Integration* load_Integration (unsigned isubint);

    //! Set all values to null
    void init ();

  };

#ifndef _PSRCHIVE_STATIC
  template<class Type>
  Registry::List<Archive::Agent>::Enter<typename Type::Agent> 
  Archive::Advocate<Type>::entry;
#endif

  /*! e.g. MyExtension* ext = archive->get<MyExtension>(); */
  template<class ExtensionType>
  const ExtensionType* Archive::get () const
  {
    const ExtensionType* extension = 0;

    for (unsigned iext=0; iext<get_nextension(); iext++) {

      const Extension* ext = get_extension (iext);

      if (verbose)
	cerr << "Pulsar::Archive::get<Ext> name=" << ext->get_name() << endl;

      extension = dynamic_cast<const ExtensionType*>( ext );

      if (extension)
	break;

    }

    return extension;
  }

  template<class ExtensionType>
  ExtensionType* Archive::get ()
  {
    const Archive* thiz = this;
    return const_cast<ExtensionType*>( thiz->get<ExtensionType>() );
  }

  /*! If the specified ExtensionType does not exist, an atempt is made to
      add it using add_extension.  If this fails, NULL is returned. */
  template<class ExtensionType>
  ExtensionType* Archive::getadd ()
  {
    const Archive* thiz = this;
    ExtensionType* retv = 0;
    retv = const_cast<ExtensionType*>( thiz->get<ExtensionType>() );

    if (retv)
      return retv;

    try {
      Reference::To<ExtensionType> add_ext = new ExtensionType;
      add_extension (add_ext);
      return add_ext;
    }
    catch (Error& error) {
      return retv;
    }

  }

}

#endif
