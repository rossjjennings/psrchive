//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Timing/Pulsar/ArrivalTime.h

#ifndef __Pulsar_ArrivalTime_h
#define __Pulsar_ArrivalTime_h

#include "Pulsar/Config.h"
#include "Pulsar/Algorithm.h"
#include "Estimate.h"
#include "toa.h"

namespace Pulsar {

  class ProfileShiftEstimator;
  class ShiftEstimator;
  class Archive;
  class Integration;
  class Profile;
  class Flux;

  //! Manages arrival time estimation
  /*! 
    This class manages the observation, template, shift estimator,
    output format, etc. thereby replacing the old Archive::toas method
    as well as parts of code from pat.
  */

  class ArrivalTime : public Algorithm {

  public:

    //! Default constructor
    ArrivalTime ();

    //! Destructor
    ~ArrivalTime ();

    //! Prepare the data for use
    virtual void preprocess (Archive* archive);

    //! Set the observation from which the arrival times will be derived
    virtual void set_observation (const Archive*);

    //! Set the standard/template used by some phase shift estimators
    virtual void set_standard (const Archive*);

    //! Set the algorithm used to estimate the phase shift
    void set_shift_estimator (ShiftEstimator*);

    //! Get the algorithm used to estimate the phase shift
    ShiftEstimator* get_shift_estimator () const;

    //! Set the format of the output time-of-arrival estimates
    void set_format (Tempo::toa::Format);

    //! Set the format as a string
    void set_format (const std::string&);

    //! Set additional format flags
    void set_format_flags (const std::string&);

    //! Set additional attributes
    void set_attributes (const std::vector< std::string >&);

    //! Set additional TOA text (only used in tempo2 format)
    void set_extra_text (const std::string&);

    //! Skip data with zero weight
    void set_skip_bad (bool flag) { skip_bad = flag; }

    //! Add to the vector of time-of-arrival estimates
    void get_toas (std::vector<Tempo::toa>&);

    //! Get auxilliary information
    std::string get_value (const std::string& key, const Tempo::toa&);

    //! Set the archive that will store residual profiles
    void set_residual (Archive* res) { residual = res; }

    //! Get the archive of residual profiles
    Archive* get_residual () { return residual; }
    
    //! Set method for computing flux density of profiles
    void set_flux_estimator (Flux *);

    //! Get flux density esimtaor
    Flux* get_flux_estimator () const;

  protected:

    //! The observation to be fit to the standard
    Reference::To<const Archive> observation;

    //! The standard to which observations are fit
    Reference::To<const Archive> standard;

    //! The residual pulse profiles (transformed observation minus standard)
    Reference::To<Archive> residual;
    
    Reference::To<ShiftEstimator> shift_estimator;

    Reference::To<Flux> flux_estimator;

    //! default TOA output format
    static Option<std::string> default_format;

    //! the output TOA format
    Tempo::toa::Format format;

    //! additional format-specific flags
    std::string format_flags;

    //! additional TOA line text
    std::string extra_text;

    //! for formats that support it, a list of attributes to add
    std::vector<std::string> attributes;
    unsigned toa_subint;
    unsigned toa_chan;

    //! when true, skip data with zero weight
    bool skip_bad;

    /*
      Perhaps Tempo::toa::Format should be a base class, such that
      each child of the class does its own formatting
    */
    std::string get_tempo2_aux_txt (const Tempo::toa&);

    /* Return the IPTA-specific tempo2 flags */
    std::string get_ipta_aux_txt (const Tempo::toa&);

    //! get the arrival times for the specified sub-integration
    virtual void get_toas (unsigned subint, std::vector<Tempo::toa>& toas);

    //! add any additional information as requested
    virtual void dress_toas (unsigned subint, std::vector<Tempo::toa>& toas);

    Tempo::toa get_toa (Estimate<double>& shift,
			const Pulsar::Integration*, unsigned ichan);

  private:

    void standard_update (unsigned ichan=0);
    void setup (const Integration* subint, unsigned ichan);

  };

}

#endif
