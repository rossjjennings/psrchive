//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ReceptionCalibrator.h,v $
   $Revision: 1.66 $
   $Date: 2005/10/11 20:26:18 $
   $Author: straten $ */

#ifndef __Pulsar_ReceptionCalibrator_H
#define __Pulsar_ReceptionCalibrator_H

#include "Pulsar/SystemCalibrator.h"
#include "Pulsar/ReflectStokes.h"

// Parameterizations of the instrument and source
#include "Calibration/StandardModel.h"
#include "Calibration/MeanCoherency.h"
#include "Calibration/CoherencyMeasurementSet.h"

#include "MEAL/NormalizeStokes.h"
#include "MEAL/Coherency.h"
#include "MEAL/VectorRule.h"

namespace Pulsar {

  class Archive;
  class Integration;
  class ReferenceCalibrator;
  class FluxCalibrator;

  class SourceEstimate {

  public:

    //! Construct with the specified bin from Archive
    SourceEstimate (unsigned ibin = 0) { phase_bin = ibin; }

    //! Update each source with the mean
    void update_source();

    //! Model of Stokes parameters as a function of frequency
    std::vector< Reference::To<MEAL::Coherency> > source;

    //! Best guess of Stokes parameters
    std::vector< Calibration::MeanCoherency > source_guess;

    //! Validity flags for each Coherency
    std::vector< bool > valid;

    //! Phase bin from which pulsar polarization is derived
    unsigned phase_bin;

    //! The index of the source in the model
    unsigned input_index;

    unsigned get_input_index() const { return input_index; }

  };


  
  //! Uses the ReceptionModel to represent and fit for the system response
  /*! The ReceptionCalibrator implements a technique of single dish
    polarimetric self-calibration.  This class requires a number of
    constraints, which are provided in through the add_observation,
    add_ReferenceCalibrator, and add_FluxCalibrator methods.
  */
  class ReceptionCalibrator : public SystemCalibrator {
    
  public:
    
    friend class ReceptionCalibratorPlotter;
    friend class SourceInfo;
    
    //! Construct with optional first pulsar archive
    ReceptionCalibrator (Calibrator::Type model, const Archive* archive = 0);

    //! Return the reference epoch of the calibration experiment
    MJD get_epoch () const;

    //! Return Calibrator::Hamaker or Calibrator::Britton
    Type get_type () const;

    //! Return the Calibrator information
    Info* get_Info () const;

    //! Return the CalibratorStokes Extension
    CalibratorStokes* get_CalibratorStokes () const;

    //! Allow the CAL Stokes V to vary (applies only if FluxCal observed)
    bool measure_cal_V;

    //! Allow the CAL Stokes Q to vary
    bool measure_cal_Q;

    //! Enforce that Stokes I > |p|, where p=(Q,U,V)
    bool physical_coherency;

    //! Normalize the Stokes parameters by the invariant interval
    bool normalize_by_invariant;

    //! Allow the gain to vary independently from observation to observation
    bool independent_gains;

    //! Print an error message if Pointing parameters are not as expected
    bool check_pointing;

    //! Reflections performed on the calibrator data immediately after loading
    ReflectStokes reflections;

    //! Add the specified pulse phase bin to the set of state constraints
    void add_state (unsigned pulse_phase_bin);
    
    //! Get the number of pulsar phase bin input polarization states
    unsigned get_nstate_pulsar () const;
    
    //! Get the total number of input polarization states (pulsar and cal)
    unsigned get_nstate () const;
    
    //! Get the number of frequency channels
    unsigned get_nchan () const;
    
    //! Set the calibrator observations to be loaded during initial_observation
    void set_calibrators (const std::vector<std::string>& filenames);
    
    //! Add the observation to the set of constraints
    void add_observation (const Archive* data);
    
    //! Add the calibrator observation to the set of constraints
    void add_calibrator (const Archive* data);
    
    //! Add the ReferenceCalibrator observation to the set of constraints
    void add_calibrator (const ReferenceCalibrator* polncal);
    
    //! Solve equation for each frequency
    void solve (int only_ichan = -1);
    
    //! Get the status of the model
    bool get_solved () const;
    
    //! Pre-calibrate the polarization of the given archive
    virtual void precalibrate (Archive* archive);
    
  protected:
    
    //! Initialize the PolnCalibration::transformation attribute
    virtual void calculate_transformation ();

    //! The calibration model as a function of frequency
    std::vector< Reference::To<Calibration::StandardModel> > model;

    //! The model specified on construction
    Calibrator::Type model_type;

    //! The platform transformation "axis"
    MEAL::Axis< Jones<double> > platform_axis;

    //! The unique transformation for each observation
    MEAL::VectorRule<MEAL::Complex2>* unique;

    //! The unique transformation "axis"
    MEAL::Axis< unsigned > unique_axis;

    //! Uncalibrated estimate of calibrator polarization
    SourceEstimate calibrator_estimate;
    
    //! Uncalibrated estimate of calibrator polarization
    SourceEstimate flux_calibrator_estimate;

    //! Uncalibrated estimate of pulsar polarization as a function of phase
    std::vector<SourceEstimate> pulsar;
    
    //! The calibrators to be loaded during initial_observation
    std::vector<std::string> calibrator_filenames;
    
    //! Routine for normalizing the Stokes parameters
    MEAL::NormalizeStokes normalizer;

    //! Epoch of the first observation
    MJD start_epoch;

    //! Epoch of the last observation
    MJD end_epoch;

    //! Minimum and maximum values of parallactic angle (informational)
    float PA_min, PA_max;

    //! Set the initial guesses and update the reference epoch
    void initialize ();

    //! Check that the model is ready to receive additional constraints
    void check_ready (const char* method, bool init = true);

    //! Initialization performed using the first observation added
    void initial_observation (const Archive* data);

    //! Load the set of calibrators set by set_calibrators
    void load_calibrators ();

    //! Add the estimate to pulsar attribute
    void init_estimate (SourceEstimate& estimate);

    void valid_mask (const SourceEstimate& src);

    //! Add Integration data to the CoherencyMeasurement vector
    /*! Data is taken from the specified frequency channel and phase bin.
      \retval bins the vector to which a new measurement will be appended
      \param estimate contains the bin number and a running mean estimate
      \param ichan the frequency channel
      \param data the Integration data
      \param variance the variance to be assigned to the measurement. 
    */
    void add_data (std::vector<Calibration::CoherencyMeasurement>& bins,
		   SourceEstimate& estimate,
		   unsigned ichan,
		   const Integration* data,
		   Stokes<float>& variance);


    //! Calibrator parameter communication
    class CalInfo : public Calibrator::Info {
      //friend class CalInfo : public Calibrator::Info {
      
    public:
      
      CalInfo (ReceptionCalibrator* cal);
      
      //! Return the number of parameter classes
      unsigned get_nclass () const;
      
      //! Return the name of the specified class
      const char* get_name (unsigned iclass) const;

      //! Return the number of parameters in the specified class
      unsigned get_nparam (unsigned iclass) const;
      
      //! Return the estimate of the specified parameter
      Estimate<float> get_param (unsigned ichan, unsigned iclass,
				 unsigned iparam) const;

      //! Return the number of channels in the calibrator estimate
      unsigned get_nchan () const;

      //! Return the number of channels in the flux calibrator estimate
      unsigned get_fcal_nchan () const;

    protected:

      //! The ReceptionCalibrator
      Reference::To<const ReceptionCalibrator> calibrator;
      
    };

    friend class CalInfo;

  private:
    //! Flag set after the solve method has been called
    bool is_fit;

    //! Flag set after the initialize method has been called
    bool is_initialized;

  };

}

#endif
