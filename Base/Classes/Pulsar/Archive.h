//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Archive.h,v $
   $Revision: 1.27 $
   $Date: 2002/04/29 11:42:41 $
   $Author: straten $ */

/*! \mainpage 
 
  \section intro Introduction
 
  The Pulsar Data Archival and Analysis Library implements a family of
  C++ classes that may be used in the storage, manipulation, and
  analysis of the observational data used in pulsar experiments.
  Various levels of functionality are organized in a heirarchy of
  inheritance; with the base classes implementing a minimal set of
  general, flexible routines.  This work is still very much under
  construction.
 
  \section profiles Pulse Profiles
 
  The basic quantity observed in most pulsar experiments is the pulse
  profile, which is implemented by the Pulsar::Profile class.  The
  Pulsar::Integration class implements a two-dimensional array of
  Pulsar::Profile objects, each integrated over the same time
  interval.  The axis of the 2-D Profile array are defined to be
  polarimetric measure and observing frequency.  The Pulsar::Archive
  class implements a one-dimensional array of Pulsar::Integration
  objects, each with similar observational parameters and each in
  "pulse phase" with eachother.

  \section minimal Minimal Interface

  The Pulsar::Profile class implements a minimal set of operations required
  to manipulate a pulsar profile.  These include:
  <UL>
  <LI> operator += - adds offset to each bin of the profile </LI>
  <LI> operator *= - multiplies each bin of the profile by scale </LI>
  <LI> rotate - rotates the profile in phase </LI>
  <LI> bscrunch - integrates neighbouring phase bins in profile </LI>
  <LI> fold - integrates neighbouring sections of the profile </LI>
  <LI> zero - sets all amplitudes to zero </LI>
  <LI> resize - resizes the data area </LI>
  </UL>

  As well, the Pulsar::Profile class implements a basic set of
  routines that may be used to calculate statistics, find minima and
  maxima, and fit to a standard.  Combinations of these functions can
  perform basic tasks.  For instance, baseline removal is simply and
  transparently implemented as:
  <pre>
  profile -= mean (find_min_phase());
  </pre>
  However, it may be decided to also implement a more convenient interface.

  The Pulsar::Subint class implements a minimal set of operations required
  to manipulate a set of Pulsar::Profile objects.  In addition to the simple
  nested calls of the above functions, these include:
  <UL>
  <LI> dedisperse - rotates all profiles to remove dispersion delays between chans </LI>
  <LI> defaraday - V_rotates all profiles to remove faraday rotation between chans </LI>
  <LI> fscrunch - integrates profiles from neighbouring chans </LI>
  <LI> pscrunch - integrates profiles from two polarizations into one total intensity </LI>
  <LI> invint - transforms from Stokes (I,Q,U,V) to the polarimetric invariant interval </LI>
  <LI> [Q|U|V]_boost - perform Lorentz boost on Stokes (I,Q,U,V) </LI>
  <LI> [Q|U|V]_rotate - perform rotation on Stokes (I,Q,U,V) </LI>
  </UL>

  The Pulsar::Archive class will be used in most high-level code.  In
  addition to providing interfaces to all of the above functions, the
  Pulsar::Archive class implements:
  <UL>
  <LI> tscrunch - integrates profiles from neighbouring Integrations </LI>
  <LI> append - copies (or transfers) the Integrations from one Archive to another </LI>
  <LI> set_ephemeris - installs a new ephemeris and polyco </LI>
  <LI> set_polyco - installs a new polynomial and aligns all profiles to it </LI>
  </UL>

  For a complete list of the methods defined in each of these base classes,
  please see the <a href="annotated.html>Compound List</a>.

  \section inheritance Inherited Types

  In general, the use of object inheritance enables:
  <UL>
  <LI> the most basic set of essential information to be defined; </LI>
  <LI> the implementation of general, powerful routines without the need of 
  various implementation details; and</LI>
  <LI> the simple extension of functionality in a manner that does not
  change the implementation of the underlying, base class methods</LI>
  </UL>

  Most observatories, instruments, and groups not only use a unique
  file format but also associate different pieces of information with
  the observation.  Some of these pieces of auxilliary information may
  need to be updated or modified during a basic operations.  For
  example, consider class B, which publicly inherits Pulsar::Archive.
  Class B may include a passband in its auxilliary data, and may wish
  to integrate passbands when one B is appended to another.  Class B
  can over-ride the virtual Pulsar::Archive::append function as follows:
  <pre>
  void B::append (Pulsar::Archive* aptr)
  {
    // call the function implemented by Pulsar::Archive
    Pulsar::archive::append (aptr);

    // dynamic_cast returns a pointer only if aptr points to an instance of B
    B* bptr = dynamic_cast<B*>(aptr);

    // test if *aptr is a B
    if (!bptr)
      return;

    // do B-specific things, such as integrate bptr->passband
  }
  </pre>

 */

#ifndef __Pulsar_Archive_h
#define __Pulsar_Archive_h

#include <vector>
#include <string>

#include "polyco.h"
#include "psrephem.h"

#include "ArchiveTypes.h"
#include "ReferenceAble.h"

namespace Tempo {
  class toa;
}

class Phase;
class psrephem;
class polyco;

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

  class Archive : public Reference::Able {

  public:

    //! A verbosity flag that can be set for debugging purposes
    static bool verbose;

    //! Flag that Archive::append should enforce chronological order
    static bool append_chronological;

    //! Amount by which integration intervals may overlap in Archive::append
    static double append_max_overlap;

    //! Flag opposite sense of sideband (upper/lower) ok in Archive::match
    static bool match_opposite_sideband;

    //! Amount by which centre frequencies may differ in Archive::match
    static double match_max_frequency_difference;

    //! Weigh integrations by their integration length, or duration
    static bool weight_by_duration;

    //! null constructor
    Archive ();

    //! destructor
    virtual ~Archive ();

    //! Loads an Archive-derived child class from a file.
    static Archive* factory (const char* filename);

    //! Convenience interface
    static Archive* factory (const string& filename)
    { return factory (filename.c_str()); }

    //! Return a pointer to a new copy of self
    virtual Archive* clone () const = 0;

    //! Return a pointer to a new fscrunched, tscrunched and pscrunched copy
    Archive* total () const;

    //! Resets the dimensions of the data area
    virtual void resize (int nsubint, int npol=0, int nchan=0, int nbin=0);

    //! Return a pointer to the integration
    Integration* get_Integration (unsigned subint);

    //! Return a pointer to the profile
    Profile* get_Profile (unsigned subint, int pol, int chan);

    // //////////////////////////////////////////////////////////////////
    //
    // virtual methods - implemented by Archive
    //
    // //////////////////////////////////////////////////////////////////

    //! Integrate pulse profiles in phase
    virtual void bscrunch (int nscrunch);

    //! Integrate neighbouring sections of the pulse profiles
    virtual void fold (int nfold);

    //! Integrate profiles in polarization
    virtual void pscrunch();

    //! Integrate profiles in frequency
    virtual void fscrunch (int nscrunch=0, bool weighted_cfreq = true);

    //! Integrate profiles in time
    virtual void tscrunch (unsigned nscrunch=0);

    //! Rotate each profile by time seconds
    virtual void rotate (double time);

    //! Append the Integrations from 'archive' to 'this'
    virtual void append (const Archive* archive);

    //! Rotate pulsar Integrations so that pulse phase zero is centred
    virtual void centre ();

    //! Rotate the Profiles to remove dispersion delays b/w chans
    virtual void dedisperse (double dm = 0.0, double frequency = 0.0);

    //! Correct the Faraday rotation of Q into U
    virtual void defaraday (double rotation_measure = 0, double rm_iono = 0);

    //! Fit Profiles to the standard and return toas
    virtual void toas (const Archive* std, vector<Tempo::toa>& toas);

    //! Correct receiver feed angle orientation
    virtual void deparallactify();

    //! Un-correct receiver feed angle orientation
    virtual void parallactify();

    //! Convert Stokes parameters to coherency products 
    virtual void ppqq();

    //! Convert coherency products to Stokes parameters 
    virtual void iquv();

    //! Transform Stokes I,Q,U,V into the polarimetric invariant interval
    virtual void invint ();
  
    //! Remove the baseline from all profiles
    virtual void remove_baseline (float phase = -1.0);

    //! Install the given ephemeris and calls update_model
    virtual void set_ephemeris (const psrephem& ephemeris);

    //! Install the given polyco and shift profiles to align
    virtual void set_model (const polyco& model);

    //! Create a new polyco and align the Integrations to the new model
    virtual void update_model ();

    //! Set the weight of each profile to its snr squared
    virtual void snr_weight ();

    // //////////////////////////////////////////////////////////////////
    //
    // convenience interfaces
    //
    // //////////////////////////////////////////////////////////////////

    //! Call bscrunch with the appropriate value
    void bscrunch_to_nbin (int new_nbin);

    //! Halve the bins
    void halvebins (int nhalve);

    //! Call fscrunch with the appropriate value
    void fscrunch_to_nchan (int new_nchan);

    //! Return the MJD at the beginning of the first sub-integration
    MJD  start_time() const;

    //! Return the MJD at the end of the last sub-integration
    MJD  end_time () const;

    //! Returns the total time integrated into all Integrations (in seconds)
    double integration_length() const;

    //! Return the coordinates of the telescope at which observation was made
    void telescope_coordinates (float* latitude = 0,
				float* longitude = 0,
				float* elevation = 0) const;

    //! Returns a block of amplitudes ordered according to the specified axis
    void get_amps (vector<float>& amps,
		   Dimension::Axis x1 = Dimension::Phase,
		   Dimension::Axis x2 = Dimension::Frequency,
		   Dimension::Axis x3 = Dimension::Poln) const;

    //! Find the transitions between high and low states in total intensity
    void find_transitions (int& hi2lo, int& lo2hi, int& buffer) const;

    //! Find the bins in which the total intensity exceeds a threshold
    void find_peak_edges (int& rise, int& fall) const;

    //! Returns the centre phase of the region with maximum total intensity
    float find_max_phase () const;

    //! Returns the centre phase of the region with minimum total intensity
    float find_min_phase () const;

    // //////////////////////////////////////////////////////////////////
    //
    // pure virtual methods - must be implemented by children
    //
    // //////////////////////////////////////////////////////////////////

    //! Write archive to disk
    virtual void unload (const char* filename) = 0;

    //! Read archive from disk
    virtual void load (const char* filename) = 0;

    //! Get the name of the thing from which the archive was loaded
    virtual string get_filename () = 0;

    //! Convenience interface to the unload function
    void unload (const string& filename) { unload (filename.c_str()); }

    //! Convenience interface to the load function
    void load (const string& filename) { load (filename.c_str()); }

    // //////////////////////////////////////////////////////////////////
    //
    // static facts about the archive
    //

    //! Get the tempo code of the telescope used
    virtual char get_tel_tempo_code () const = 0;

    //! Get the feed configuration of the receiver
    virtual Feed::Type get_feed_type () const = 0;

    //! Get the observation type (psr, cal)
    virtual Observation::Type get_observation_type () const = 0;

    //! Get the source name
    virtual string get_source () const = 0;

    // //////////////////////////////////////////////////////////////////
    //
    // dynamic facts about the archive
    //

    //! Get the number of pulsar phase bins used
    /*! This attribute may be set only through Archive::resize */
    virtual int get_nbin () const = 0;

    //! Get the number of frequency channels used
    /*! This attribute may be set only through Archive::resize */
    virtual int get_nchan () const = 0;

    //! Get the number of frequency channels used
    /*! This attribute may be set only through Archive::resize */
    virtual int get_npol () const = 0;

    //! Get the number of sub-integrations stored in the file
    /*! This attribute may be set only through Archive::resize */
    virtual int get_nsubint () const { return subints.size(); }

    //! Get the overall bandwidth of the observation
    virtual double get_bandwidth () const = 0;
    //! Set the overall bandwidth of the observation
    virtual void set_bandwidth (double bw) = 0;

    //! Get the centre frequency of the observation
    virtual double get_centre_frequency () const = 0;
    //! Set the centre frequency of the observation
    virtual void set_centre_frequency (double cf) = 0;

    //! Get the state of the profiles
    virtual Poln::State get_poln_state () const = 0;
    //! Set the state of the profiles
    virtual void set_poln_state (Poln::State state) = 0;

    //! Get the centre frequency of the observation
    virtual double get_dispersion_measure () const = 0;
    //! Set the centre frequency of the observation
    virtual void set_dispersion_measure (double dm) = 0;


    // //////////////////////////////////////////////////////////////////
    //
    // various state flags
    //

    //! Data has been corrected for feed angle errors
    virtual bool get_feedangle_corrected () const = 0;
    //! Set the status of the feed angle flag
    virtual void set_feedangle_corrected (bool done = true) = 0;

    //! Data has been corrected for ionospheric faraday rotation
    virtual bool get_iono_rm_corrected () const = 0;
    //! Set the status of the ionospheric RM flag
    virtual void set_iono_rm_corrected (bool done = true) = 0;

    //! Data has been corrected for ISM faraday rotation
    virtual bool get_ism_rm_corrected () const = 0;
    //! Set the status of the ISM RM flag
    virtual void set_ism_rm_corrected (bool done = true) = 0;

    //! Data has been corrected for parallactic angle errors
    virtual bool get_parallactic_corrected () const = 0;
    //! Set the status of the parallactic angle flag
    virtual void set_parallactic_corrected (bool done = true) = 0;

    //! Test if arch matches (enough for a pulsar - calibrator match)
    virtual bool match (const Archive* arch, string& reason);

    //! Test if arch is mixable (enough for append)
    virtual bool mixable (const Archive* arch, string& reason);

    //! Computes the weighted channel frequency of an interval of subints.
    double weighted_frequency (int ichan, int start, int end) const;

 protected:

    //! The pulsar ephemeris, as used by TEMPO
    psrephem ephemeris;

    //! The pulsar phase model, as created using TEMPO
    polyco model;

    //! The data storage area
    vector<Integration*> subints;

    //! All new Integration instances are created through this method
    virtual Integration* new_Integration (Integration* subint = 0) = 0;

    //! Set the number of pulsar phase bins
    /*! Called by Archive methods to update child attribute */
    virtual void set_nbin (int numbins) = 0;

    //! Set the number of frequency channels
    /*! Called by Archive methods to update child attribute */
    virtual void set_nchan (int numchan) = 0;

    //! Set the number of polarization measurements
    /*! Called by Archive methods to update child attribute */
    virtual void set_npol (int numpol) = 0;

    //! Set the number of sub-integrations
    /*! Called by Archive methods to update child attribute */
    virtual void set_nsubint (int num_sub) { }

    //! Initialize an Integration to reflect Archive attributes.
    void init_Integration (Integration* subint);

    //! Set all values to null
    void init ();

    //! Append clones of Integration objects to subints
    void append (const vector<Integration*>& more_subints);
  
    //! Apply the current model to the Integration
    void apply_model (const polyco& old, Integration* subint);

    //! Update the polyco model and correct the Integration set
    void update_model (unsigned old_nsubint);

    //! Creates polynomials to span the Integration set
    void create_updated_model (bool clear_old);

    //! Returns true if the given model spans the Integration set
    bool good_model (const polyco& test_model) const;

  private:

    //! This flag may be raised only by Archive::update_model.
    /*!
      As it is set only during run-time, this flag makes it known that
      the current Integration set has been aligned to a polyco created
      during run-time
    */
    bool model_updated;

  };

}

#endif










