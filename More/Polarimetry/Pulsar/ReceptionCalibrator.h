//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ReceptionCalibrator.h,v $
   $Revision: 1.48 $
   $Date: 2003/12/02 14:27:19 $
   $Author: straten $ */

#ifndef __ReceptionCalibrator_H
#define __ReceptionCalibrator_H

#include "Pulsar/PolnCalibrator.h"

// Reception Model and its management
#include "Calibration/ReceptionModel.h"
#include "Calibration/ConvertMJD.h"
#include "Calibration/Axis.h"

// Parameterizations of the instrument and source
#include "Calibration/PolarEstimate.h"
#include "Calibration/SingleAxisEstimate.h"
#include "Calibration/StokesEstimate.h"
#include "Calibration/NormalizeStokes.h"

#include "Calibration/Instrument.h"
#include "Calibration/Polar.h"

// Extra transformations
#include "Calibration/SingleAxis.h"
#include "Calibration/Parallactic.h"

namespace Pulsar {

  class Archive;
  class Integration;
  class ArtificialCalibrator;
  class FluxCalibrator;

  class SourceEstimate {

  public:

    //! Construct with the specified bin from Archive
    SourceEstimate (unsigned ibin = 0) { phase_bin = ibin; }

    //! Update each source with the mean
    void update_source();

    //! Model of Stokes parameters added to equation as a function of frequency
    vector< Calibration::StokesEstimate > source;

    //! Validity flags for each StokesEstimate
    vector< bool > valid;

    //! Phase bin from which pulsar polarization is derived
    unsigned phase_bin;

    //! The index of the source in the model
    unsigned source_index;

  };

  //! Stores the various elements related to the calibration model
  class StandardModel : public Reference::Able {

  public:
    
    friend class ReceptionCalibrator;

    //! Construct
    StandardModel (Calibrator::Type model = Calibrator::Hamaker);

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
    Reference::To<Calibration::ProductTransformation> pcal_path;

    //! The signal path experienced by the pulsar
    Reference::To<Calibration::ProductTransformation> pulsar_path;

    //! The instrumental model and any additional transformations
    Reference::To<Calibration::ProductTransformation> instrument;

    // ////////////////////////////////////////////////////////////////////
    //
    //! Polar decomposition of instrumental response (Hamaker)
    Reference::To<Calibration::Polar> polar;

    //! The best estimate of the polar model
    Calibration::PolarEstimate polar_estimate;

    // ////////////////////////////////////////////////////////////////////
    //
    //! Phenomenological decomposition of instrumental response (Britton)
    Reference::To<Calibration::Instrument> physical;

    //! The best estimate of the physical model
    Calibration::SingleAxisEstimate physical_estimate;

    // ////////////////////////////////////////////////////////////////////
    //
    //! Additional backend required for flux calibrator signal path
    Reference::To<Calibration::SingleAxis> fluxcal_backend;

    //! The best estimate of the flux calibration backend
    Calibration::SingleAxisEstimate fluxcal_backend_estimate;

    // ////////////////////////////////////////////////////////////////////
    //
    //! The parallactic angle rotation
    Calibration::Parallactic parallactic;

    //! The time axis
    Calibration::Axis<MJD> time;

    //! Used to convert MJD to double
    Calibration::ConvertMJD convert;

    //! The signal path of the FluxCalibrator source
    unsigned FluxCalibrator_path;

    //! The signal path of the ArtificialCalibrator source
    unsigned ArtificialCalibrator_path;

    //! The signal path of the Pulsar phase bin sources
    unsigned Pulsar_path;

  protected:
    //! The model specified on construction
    Calibrator::Type model;

    //! validity flag
    bool valid;
  };
  
  
  //! Uses the ReceptionModel to represent and fit for the system response
  /*! The ReceptionCalibrator implements a technique of single dish
    polarimetric self-calibration.  This class requires a number of
    constraints, which are provided in through the add_observation,
    add_ArtificialCalibrator, and add_FluxCalibrator methods.
  */
  class ReceptionCalibrator : public PolnCalibrator {
    
  public:
    
    friend class ReceptionCalibratorPlotter;
    
    //! Construct with optional first pulsar archive
    ReceptionCalibrator (Calibrator::Type model,
			 const Archive* archive = 0);
    
    //! Return Calibrator::Hamaker or Calibrator::Britton
    Type get_type () const;

    //! Return the Calibrator information
    Calibrator::Info* get_Info () const;

    //! Return a PolnCalibrator::get_solution with a CalibratorStokes Extension
    Archive* get_solution (const string& archive_type, string ext=".pc") const;

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
    
    //! Add the ArtificialCalibrator observation to the set of constraints
    void add_Calibrator (const ArtificialCalibrator* polncal);
    
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
    Calibration::NormalizeStokes normalizer;

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

    //! Add Integration data to the MeasuredState vector
    /*! Data is taken from the specified frequency channel and phase bin.
      \retval bins the vector to which a new MeasuredState will be appended
      \param estimate contains the bin number and a running mean estimate
      \param ichan the frequency channel
      \param data the Integration data
      \param variance the variance to be assigned to the MeasuredState. 
    */
    void add_data (vector<Calibration::MeasuredState>& bins,
		   SourceEstimate& estimate,
		   unsigned ichan,
		   const Integration* data,
		   Stokes<float>& variance);


    //! Calibrator parameter communication
    friend class CalInfo : public Calibrator::Info {
      
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

  private:
    //! Flag set after the solve method has been called
    bool is_fit;

    //! Flag set after the initialize method has been called
    bool is_initialized;

  };

}

#endif
