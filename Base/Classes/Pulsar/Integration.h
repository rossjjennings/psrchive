//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Integration.h,v $
   $Revision: 1.64 $
   $Date: 2004/07/16 07:29:05 $
   $Author: straten $ */

/*
  
  Pulsar::Integration - base class for pulsar observations

*/

#ifndef __Pulsar_Integration_h
#define __Pulsar_Integration_h

#include <vector>

#include "MJD.h"
#include "Types.h"
#include "Reference.h"
#include "Estimate.h"
#include "Jones.h"

#include "toa.h"

template<typename T> class Stokes;

namespace Pulsar {

  class Profile;
  class PolnProfile;
  class Archive;

  //! Group of Pulsar::Profile objects integrated over the same time interval
  class Integration : public Reference::Able {
    
  public:
    
    //! flag controls the amount output to stderr by Integration methods
    static bool verbose;

    //! Maximum centre frequency difference in Integration::mixable
    static double match_max_frequency_difference;

    //! Default constructor
    Integration ();

    //! Copy constructor
    Integration (const Integration& subint);

    //! Assignment operator
    Integration& operator = (const Integration& subint);
    
    //! Destructor
    virtual ~Integration ();

    //! Test if integ is mixable (enough for combination with +=)
    virtual bool mixable (const Integration* integ, string& reason) const;

    //! Get the MJD at the start of the integration (convenience interface)
    MJD get_start_time () const;

    //! Get the MJD at the end of the integration (convenience interface)
    MJD get_end_time () const;

    //! Get the Profile centre frequency attribute of the given channel
    virtual double get_centre_frequency (unsigned ichan) const;
    //! Set the Profile centre frequency attributes of the given channel
    virtual void set_centre_frequency (unsigned ichan, double frequency);

    //! Get the Profile weight attribute of the given channel
    virtual float get_weight (unsigned ichan) const;
    //! Set the Profile weight attributes of the given channel
    virtual void set_weight (unsigned ichan, float weight);

    //! Get flux
    float flux (int _poln = 0, float dc = 0.15);
    
    //! Get polarized flux
    float get_poln_flux (int _type = 0,  int startbin = 0, int stopbin = 0);

    //! Find the transitions between high and low states in total intensity
    void find_transitions (int& hi2lo, int& lo2hi, int& buffer) const;

    //! Find the bins in which the total intensity exceeds a threshold
    void find_peak_edges (int& rise, int& fall) const;

    //! Returns the centre phase of the region with maximum total intensity
    float find_max_phase () const;

    //! Returns the centre phase of the region with minimum total intensity
    float find_min_phase (float dc = 0.15) const;

    //! Return the statistics of every profile baseline
    void baseline_stats (vector< vector< Estimate<double> > >* mean,
			 vector< vector<double> >* variance = 0) const;

    //! Returns the mean hi/lo and variance of the mean hi/lo of every profile
    virtual void cal_levels (vector< vector< Estimate<double> > >& hi,
			     vector< vector< Estimate<double> > >& lo) const;

    void find_psr_levels (vector<vector<double> >& mean_high,
			  vector<vector<double> >& mean_low) const;

    //! Computes the weighted centre frequency of an interval of sub-chans.
    double weighted_frequency (unsigned ch_start=0, unsigned ch_end=0) const;
    
    //! Return a vector of tempo++ toa objects
    void toas (vector<Tempo::toa>& toas, const Integration& std_subint,
	       char nsite, string arguments = "", 
	       bool time_domain = false, 
	       Tempo::toa::Format fmt = Tempo::toa::Parkes,
	       bool discard_bad = false) const;
    
    //! Remove the baseline from all profiles
    virtual void remove_baseline (float phase = -1.0, float dc = 0.15);

    //! Set the weight of each profile to its snr squared
    void snr_weight ();
    
    //! Set the weight of each profile to the given number
    void uniform_weight (float new_weight = 1.0);
    

    // //////////////////////////////////////////////////////////////////
    //
    // Copying and cloning
    //
    // //////////////////////////////////////////////////////////////////

    /** @name Copying and Cloning
     *
     *  The clone and total methods return pointers to new
     *  copy-constructed instances.
     */
    //@{

    //! Return pointer to a new copy of self
    virtual Integration* clone (int npol=-1, int nchan=-1) const = 0;

    //! Return the pointer to a new fscrunched and pscrunched copy of self
    Integration* total () const;
    
    //@}


    // //////////////////////////////////////////////////////////////////
    //
    // Dimension Attributes
    //
    // //////////////////////////////////////////////////////////////////

    /** @name Dimension Attributes
     *
     *  These pure virtual methods return the dimension attributes stored 
     *  by derived classes.  Note that the dimensions of an Integration may be 
     *  changed only through the resize method.
     */
    //@{

    //! Get the number of chans
    /*! This attribute may be set only through Integration::resize */
    virtual unsigned get_nchan () const = 0;

    //! Get the number of polarization measurements
    /*! This attribute may be set only through Integration::resize */
    virtual unsigned get_npol () const = 0;

    //! Get the number of bins in each profile
    /*! This attribute may be set only through Integration::resize */
    virtual unsigned get_nbin () const = 0;
 
    //@}


    // //////////////////////////////////////////////////////////////////
    //
    // Integration Unique Attributes
    //
    // //////////////////////////////////////////////////////////////////

    /** @name Integration Unique Attributes
     *
     *  These pure virtual methods provide access to the attributes that 
     *  are unique to each Integration.
     */
    //@{

    //! Get the epoch of the rising edge of bin zero
    virtual MJD get_epoch () const = 0;
    //! Set the epoch of the rising edge of bin zero
    /*  When not dealing with single-pulse data, the epoch should
       refer to that of the rising edge of phase bin zero near the
       middle of the integration. */
    virtual void set_epoch (const MJD& mjd) = 0;

    //! Get the total time integrated (in seconds)
    virtual double get_duration() const =0;
    //! Set the total time integrated (in seconds)
    virtual void set_duration (double seconds) = 0;

    //! Get the folding or topocentric pulsar period (in seconds)
    virtual double get_folding_period() const = 0;
    //! Set the folding or topocentric pulsar period (in seconds)
    /*  The topocentric folding period of the pulsar should be equal
	to that at the epoch defined by get_epoch. */
    virtual void set_folding_period (double seconds) = 0;

    //@}


    // //////////////////////////////////////////////////////////////////
    //
    // Archive Shared Attributes
    //
    // //////////////////////////////////////////////////////////////////

    /** @name Archive Shared Attributes
     *
     *  By default, each Integration has the same observing parameters
     *  as all other Integrations in the Archive; therefore, it is not
     *  possible to set these attributes through the Integration base
     *  class interface. */
    //@{

    //! Get the centre frequency (in MHz)
    virtual double get_centre_frequency() const;
    
    //! Get the bandwidth (in MHz)
    virtual double get_bandwidth() const;

    //! Get the dispersion measure (in \f${\rm pc\, cm}^{-3}\f$)
    virtual double get_dispersion_measure () const;
    
    //! Inter-channel dispersion delay has been removed
    virtual bool get_dedispersed () const;

    //! Get the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
    virtual double get_rotation_measure () const;

    //! Data has been corrected for ISM faraday rotation
    virtual bool get_faraday_corrected () const;

    //! Get the feed configuration of the receiver
    virtual Signal::Basis get_basis () const;

    //! Get the polarimetric state of the profiles
    virtual Signal::State get_state () const;

    //@}


    // //////////////////////////////////////////////////////////////////
    //
    // Access to data
    //
    // //////////////////////////////////////////////////////////////////

    /** @name Data Access
     *
     *  These methods provide access to the data contained in the Integration
     */
    //@{

    //! Returns a pointer to the Profile given by the specified indeces
    Profile* get_Profile (unsigned ipol, unsigned ichan);

    //! Returns a const pointer to the Profile given by the specified indeces
    const Profile* get_Profile (unsigned ipol, unsigned ichan) const;

    //! Returns a pointer to a new PolnProfile instance
    PolnProfile* new_PolnProfile (unsigned ichan);

    //! Returns a const pointer to a new PolnProfile instance
    const PolnProfile* new_PolnProfile (unsigned ichan) const;

    //! Return the Stokes 4-vector for the frequency channel and phase bin
    Stokes<float> get_Stokes (unsigned ichan, unsigned ibin) const;

    //! Returns a vector of Stokes parameters along the specified dimension
    void get_Stokes (vector< Stokes<float> >& S, unsigned iother,
		     Signal::Dimension abscissa = Signal::Phase ) const;


    // //////////////////////////////////////////////////////////////////
    //
    // Extension access
    //
    // //////////////////////////////////////////////////////////////////

    /** @name Extension Interface 
     *
     * Derived classes can provide access to the additional
     * information available in their associated file format through
     * use of Extension classes. 
     */
    //@{
    
    //! Abstract base class of Integration::Extension objects
    /* Integration-derived classes may provide access to additional 
       information through Extension-derived objects. */
    class Extension : public Reference::Able {
      
    public:
      
      //! Construct with a name
      Extension (const char* name);
      
      //! Destructor
      virtual ~Extension ();
      
      //! Return a new copy-constructed instance identical to this instance
      virtual Extension* clone () const = 0;
      
      //! Append another of self, combining data in a sensible way
      /*! Derived classes should ensure that a dynamic cast is
	performed to ensure that the two objects are of the same
	type and return immediately if they are not.
      */
      virtual void append (Extension* ext) = 0;
      
      //! Return the name of the Extension
      string get_name () const;
      
    protected:
      
      //! Extension name - useful when debugging
      string name;
      
    };
    
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

    //! Add an Extension to the Integration instance
    /*! The derived class must ensure that only one instance of the Extension
      type is stored.

      \return On successful addition, this method should return the
      pointer to the Extension, equal to the extension argument.  If the
      Extension is not supported, this method should return a null pointer.
    */
    virtual void add_extension (Extension* extension);

    //@}

  protected:

    // //////////////////////////////////////////////////////////////////
    //
    // Dimension Attributes
    //
    // //////////////////////////////////////////////////////////////////

    /** @name Dimension Attributes
     *
     *  The pure virtual methods are used by the Integration class to
     *  set the dimension attributes stored by the derived classes.
     *  Note that the dimensions of an Integration should be changed
     *  only through the resize method.
     */
    //@{

    //! Set the number of pulsar phase bins
    virtual void set_nbin (unsigned nbin) = 0;

    //! Set the number of frequency channels
    virtual void set_nchan (unsigned nchan) = 0;

    //! Set the number of polarization measurements
    virtual void set_npol (unsigned npol) = 0;

    //! Set the dimensions of the data container
    virtual void resize (unsigned npol=0, unsigned nchan=0, unsigned nbin=0);

    //@}

    friend class Archive;
    friend class BinaryPhaseOrder;
    friend class PeriastronOrder;
    friend class BinLngPeriOrder;
    friend class BinLngAscOrder;
    friend class Calibrator;
    friend class Plotter;

    //! Copy the profiles and attributes through set_ get_ methods
    virtual void copy (const Integration& subint, int npol=-1, int nchan=-1);

    //! Call Profile::fold on every profile
    virtual void fold (unsigned nfold);

    //! Call Profile::bsrunch on every profile
    virtual void bscrunch (unsigned nscrunch);
    
    //! Rotate each profile by time (in seconds)
    virtual void rotate (double time);

    //! Integrate profiles from neighbouring chans
    virtual void fscrunch (unsigned nscrunch = 0);

    //! Integrate profiles from single polarizations into one total intensity
    virtual void pscrunch ();
    
    //! operator +=
    /*! This operator is provided for experimental purposes and should
      not be used lightly. If the time spans of the two combined
      subints are not contiguous, several parameters in the result are
      no longer meaningful. Also, there is no way of knowing whether
      the profiles are aligned in phase. Developers who use this
      operator should perform the necessary rotations at the archive
      level (where the polyco resides) before summing the data in the
      Integrations. */
    void operator += (const Integration& subint);
    
    //! Append frequency channels from another Integration
    /*!  Note that this is dangerous and only intended for use with instruments 
      whose band is split into adjoining segments (like cpsr2) */
    void fappend (Pulsar::Integration* integ, bool ignore_time_mismatch = false);

    //! Remove inter-channel dispersion delays with respect to centre frequency
    virtual void dedisperse ();

    //! Remove Faraday rotation with respect to centre frequency
    virtual void defaraday ();
    
    //! Transform from Stokes (I,Q,U,V) to the polarimetric invariant interval
    virtual void invint ();
    
    //! Perform the congruence transformation on each polarimetric profile
    void transform (const Jones<float>& response);

    //! Perform frequency response on each polarimetric profile
    void transform (const vector< Jones<float> >& response);

    //! get PA as a function of phase
    virtual void get_PA (vector<double> &phases, vector<double> &angles,
			 vector<double> &errors, float _threshold=2.5);

    //! Convert polarimetric data to the specified state
    virtual void convert_state (Signal::State state);

    //! All new Profile instances are created through this method
    virtual Profile* new_Profile ();

    //! The Extensions added to this Integration instance
    vector< Reference::To<Extension> > extension;

    //! Data: npol by nchan profiles
    vector< vector< Reference::To<Profile> > > profiles;

    //! The Archive that manages this integration
    Reference::To<const Archive, false> archive;

  private:

    //! Converts between coherency products and Stokes parameters
    void poln_convert (Signal::State out_state);

    //! Dedispersion worker function
    void dedisperse (unsigned ichan, unsigned kchan,
                     double dm, double f0, double t0 = 0);

    //! Defaraday worker function
    void defaraday (unsigned ichan, unsigned kchan,
                    double rm, double f0, const Jones<double>& r0 = 1);

    //! Record the rotation measure applied in defaraday
    double defaradayed_rotation_measure;

    //! Record the centre frequency applied in defaraday
    double defaradayed_centre_frequency;

    //! Record the rotation measure applied in defaraday
    double dedispersed_dispersion_measure;

    //! Record the centre frequency applied in defaraday
    double dedispersed_centre_frequency;

  };

  /*! e.g. MyExtension* ext = integration->get<MyExtension>(); */
  template<class ExtensionType>
  const ExtensionType* Integration::get () const
  {
    const ExtensionType* extension = 0;

    for (unsigned iext=0; iext<get_nextension(); iext++) {

      const Extension* ext = get_extension (iext);

      if (verbose)
	cerr << "Pulsar::Integration::get<Ext> name=" << ext->get_name() << endl;

      extension = dynamic_cast<const ExtensionType*>( ext );

      if (extension)
	break;

    }

    return extension;
  }

  template<class ExtensionType>
  ExtensionType* Integration::get ()
  {
    const Integration* thiz = this;
    return const_cast<ExtensionType*>( thiz->get<ExtensionType>() );
  }

}

#endif
