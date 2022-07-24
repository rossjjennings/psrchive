//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/FluxCalibrator.h

#ifndef __Pulsar_FluxCalibrator_H
#define __Pulsar_FluxCalibrator_H

#include "Pulsar/Calibrator.h"

namespace Pulsar {

  class Integration;
  class StandardCandles;
  class CalibratorStokes;
  class FluxCalibratorExtension;
  
  //! Calibrates flux using standard candles and artificial sources
  class FluxCalibrator : public Calibrator {
    
  public:

    //! Default constructor
    FluxCalibrator (const Archive* archive = 0);

    //! Destructor
    ~FluxCalibrator ();

    //! Virtual base class of flux calibration policies
    class Policy;

    //! Policy used when gain varies between FluxCal-On and FluxCal-Off
    class VariableGain;

    //! Policy used when gain is constant, but may be impacted by noise diode
    class ConstantGain;

    //! Set the policy used to perform flux calibration
    void set_policy (Policy*);
		     
    //! FluxCalibrator parameter communication
    class Info;

    //! Return the FluxCalibrator information
    Calibrator::Info* get_Info () const;
    
    //! Return a new FluxCalibratorExtension
    CalibratorExtension* new_Extension () const;

    //! Return information about the standard candle used
    std::string get_standard_candle_info () const;

    //! Return the system temperature in Kelvin
    double meanTsys ();
    
    //! Return the system temperature of a specific channel
    double Tsys (unsigned ichan);

    //! Return an estimate of the artificial cal Stokes parameters
    const CalibratorStokes* get_CalibratorStokes () const;
    
    //! Add a FluxCal Pulsar::Archive to the set of constraints
    void add_observation (const Archive* archive);

    //! Set the database containing flux calibrator information
    void set_database (const StandardCandles* database);

    //! Set the threshold used to reject outliers when computing levels
    void set_outlier_threshold (float f) { outlier_threshold = f; }

    //! Get the threshold used to reject outliers when computing levels
    float get_outlier_threshold () const { return outlier_threshold; }

    //! Calibrate the flux in the given archive
    void calibrate (Archive* archive);

    //! Get the number of frequency channels in the calibrator
    unsigned get_nchan () const;

    //! Get the number of receptors in the calibrator
    unsigned get_nreceptor () const;

    //! Return true if the flux scale for the specified channel is valid
    bool get_valid (unsigned ch) const;

    //! Return the weight (0 or 1) associated with the specified channel
    float get_weight (unsigned ichan) const;

    //! Set the flux scale invalid flag for the specified channel
    void set_invalid (unsigned ch);

    //! Print all the fluxcal info
    void print(std::ostream& os=std::cout);

    //! Check whether both on and off are present
    bool complete () const { return (have_on && have_off); }

    //! Setup to calibrate the flux in the given archive
    void calibration_setup (const Archive* archive);

  protected:

    friend class FluxCalibratorExtension;
    friend class ConstantGainInfo;
    friend class Info;

    //! Flux calibrator database
    Reference::To<const StandardCandles> database;

    //! Flux calibrator extension
    Reference::To<const FluxCalibratorExtension> flux_extension;
    
    //! Create the cal_flux spectrum at the requested resolution
    void create (unsigned nchan = 0);

    //! Calibrate a single sub-integration
    void calibrate (Integration* subint);

    //! Flux calibrator data for each frequency channel
    std::vector< Reference::To<Policy> > data;

    //! Check on data range and validity
    void data_range_check (unsigned ichan, const char* method) const;

    //! Resize the data vector
    void resize (unsigned nchan, unsigned nreceptor);

    //! The absolute gain used to calibrate the archive data
    std::vector< float > gain;

    //! Resize the gain vector
    void resize (unsigned required_nchan);

    //! The policy used to peform flux calibration
    Reference::To<Policy> policy;
    
  private:

    //! The CalibratorStokes parameters derived from this solution
    mutable Reference::To<CalibratorStokes> calibrator_stokes;
    
    //! Set true after call to calculate
    bool calculated;

    //! FluxCal-On data available
    bool have_on;

    //! FluxCal-Off data available
    bool have_off;

    //! Information stored about the standard candle
    std::string standard_candle_info;

    //! Threshold used to reject outliers when computing levels
    double outlier_threshold;

    //! Assume that pulsar and standard candle were observed on same scale
    bool constant_scale;

    //! Initialize attributes
    void init ();

    //! Set up the data array with standard candle flux density
    void setup ();

  };

}

#endif
