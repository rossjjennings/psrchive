//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ReceptionCalibrator.h,v $
   $Revision: 1.60 $
   $Date: 2004/11/22 21:32:30 $
   $Author: straten $ */

#ifndef __Pulsar_ReceptionCalibrator_H
#define __Pulsar_ReceptionCalibrator_H

#include "Pulsar/SystemCalibrator.h"

// Reception Model and its management
#include "Calibration/ReceptionModel.h"
#include "Calibration/ConvertMJD.h"
#include "MEAL/Axis.h"

// Parameterizations of the instrument and source
#include "Calibration/MeanPolar.h"
#include "Calibration/MeanSingleAxis.h"
#include "Calibration/MeanCoherency.h"
#include "Calibration/CoherencyMeasurementSet.h"
#include "Calibration/Instrument.h"

#include "MEAL/NormalizeStokes.h"
#include "MEAL/Polar.h"
#include "MEAL/Coherency.h"

// Extra transformations
#include "Calibration/SingleAxis.h"
#include "Calibration/Parallactic.h"

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

    //! Model of Stokes parameters added to equation as a function of frequency
    vector< MEAL::Coherency > source;

    //! Best guess of Stokes parameters
    vector< Calibration::MeanCoherency > source_guess;

    //! Validity flags for each Coherency
    vector< bool > valid;

    //! Phase bin from which pulsar polarization is derived
    unsigned phase_bin;

    //! The index of the source in the model
    unsigned input_index;

    unsigned get_input_index() const { return input_index; }

  };

  //! Stores the various elements related to the calibration model
  class StandardModel : public Reference::Able {

  public:
    
    friend class ReceptionCalibrator;

    //! Constructor
    StandardModel (Calibrator::Type model = Calibrator::Hamaker,
                   MEAL::Complex2* feed_corrections = 0);

    //! Update the relevant estimate
    void update ();

    //! Add a new signal path for the poln calibrator observations
    void add_polncal_backend ();

    //! Add a new signal path for the flux calibrator observations
    void add_fluxcal_backend ();

    //! Fix the rotation about the line of sight
    void fix_orientation ();

    //! ReceptionModel
    Reference::To<Calibration::ReceptionModel> equation;

    //! The signal path experienced by the calibrator
    Reference::To<MEAL::ProductRule<MEAL::Complex2> > pcal_path;

    //! The signal path experienced by the pulsar
    Reference::To<MEAL::ProductRule<MEAL::Complex2> > pulsar_path;

    //! The instrumental model and any additional transformations
    Reference::To<MEAL::ProductRule<MEAL::Complex2> > instrument;

    // ////////////////////////////////////////////////////////////////////
    //
    //! Polar decomposition of instrumental response (Hamaker)
    Reference::To<MEAL::Polar> polar;

    //! The best estimate of the polar model
    Calibration::MeanPolar polar_estimate;

    // ////////////////////////////////////////////////////////////////////
    //
    //! Phenomenological decomposition of instrumental response (Britton)
    Reference::To<Calibration::Instrument> physical;

    //! The best estimate of the physical model
    Calibration::MeanSingleAxis physical_estimate;

    // ////////////////////////////////////////////////////////////////////
    //
    //! Additional backend required for flux calibrator signal path
    Reference::To<Calibration::SingleAxis> fluxcal_backend;

    //! The best estimate of the flux calibration backend
    Calibration::MeanSingleAxis fluxcal_backend_estimate;

    // ////////////////////////////////////////////////////////////////////
    //
    //! The parallactic angle rotation
    Calibration::Parallactic parallactic;

    //! The time axis
    MEAL::Axis<MJD> time;

    //! Used to convert MJD to double
    Calibration::ConvertMJD convert;

    //! The signal path of the FluxCalibrator source
    unsigned FluxCalibrator_path;

    //! The signal path of the ReferenceCalibrator source
    unsigned ReferenceCalibrator_path;

    //! The signal path of the Pulsar phase bin sources
    unsigned Pulsar_path;

  protected:

    //! The model specified on construction
    Calibrator::Type model;

    //! The feed correction transformation
    Reference::To<MEAL::Complex2> feed_correction;

    //! validity flag
    bool valid;

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
    ReceptionCalibrator (Calibrator::Type model,
			 const Archive* archive = 0);

    //! Return the reference epoch of the calibration experiment
    MJD get_epoch () const;

    //! Return Calibrator::Hamaker or Calibrator::Britton
    Type get_type () const;

    //! Return the Calibrator information
    Info* get_Info () const;

    //! Return the CalibratorStokesExtension
    CalibratorStokes* get_calibrator_stokes () const;

    //! Allow the CAL Stokes V to vary (applies only if FluxCal observed)
    bool measure_cal_V;
    //! Allow the CAL Stokes Q to vary
    bool measure_cal_Q;

    bool normalize_by_invariant;

    //! Add the specified pulse phase bin to the set of state constraints
    void add_state (unsigned pulse_phase_bin);
    
    //! Get the number of pulsar phase bin input polarization states
    unsigned get_nstate_pulsar () const;
    
    //! Get the total number of input polarization states (pulsar and cal)
    unsigned get_nstate () const;
    
    //! Get the number of frequency channels
    unsigned get_nchan () const;
    
    //! Set the calibrator observations to be loaded during initial_observation
    void set_calibrators (const vector<string>& filenames);
    
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
    vector< Reference::To<StandardModel> > model;

    //! The model specified on construction
    Calibrator::Type model_type;

    //! Uncalibrated estimate of calibrator polarization
    SourceEstimate calibrator_estimate;
    
    //! Uncalibrated estimate of calibrator polarization
    SourceEstimate flux_calibrator_estimate;

    //! Uncalibrated estimate of pulsar polarization as a function of phase
    vector<SourceEstimate> pulsar;
    
    //! The calibrators to be loaded during initial_observation
    vector<string> calibrator_filenames;
    
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
    void add_data (vector<Calibration::CoherencyMeasurement>& bins,
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
