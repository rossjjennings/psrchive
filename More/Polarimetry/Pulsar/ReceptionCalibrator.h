//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ReceptionCalibrator.h,v $
   $Revision: 1.35 $
   $Date: 2003/09/03 13:52:10 $
   $Author: straten $ */

#ifndef __ReceptionCalibrator_H
#define __ReceptionCalibrator_H

#include "Calibrator.h"

// Reception Model and its management
#include "Calibration/ReceptionModel.h"
#include "Calibration/Axis.h"

// Parameterizations of the instrument and source
#include "Calibration/PolarEstimate.h"
#include "Calibration/InstrumentEstimate.h"
#include "Calibration/StokesEstimate.h"

// Extra transformations
#include "Calibration/SingleAxis.h"
#include "Calibration/Parallactic.h"

namespace Pulsar {

  class Archive;
  class Integration;
  class PolnCalibrator;
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

    //! The available representations of the instrumental response
    enum Model { Hamaker, Britton };

    //! Construct
    StandardModel (Model model = Hamaker);

    //! Update the relevant estimate
    void update ();

    //! ReceptionModel
    Reference::To<Calibration::ReceptionModel> equation;

    //! The signal path experienced by the calibrator
    Reference::To<Calibration::ProductTransformation> pcal_path;

    //! The signal path experienced by the pulsar
    Reference::To<Calibration::ProductTransformation> pulsar_path;

    //! The instrumental model in use
    Reference::To<Calibration::Transformation> instrument;

    //! Polar decomposition of instrumental response (Hamaker)
    Reference::To<Calibration::PolarEstimate> polar;

    //! Phenomenological decomposition of instrumental response (Britton)
    Reference::To<Calibration::InstrumentEstimate> physical;

    //! Single-axis decomposition of backend
    Reference::To<Calibration::SingleAxis> backend;

    //! The signal path of the PolnCalibrator sources
    unsigned PolnCalibrator_path;

    //! The signal path of the Pulsar sources
    unsigned Pulsar_path;

  protected:
    //! The model specified on construction
    Model model;

    //! validity flag
    bool valid;
  };
  
  
  //! Uses the ReceptionModel to represent and fit for the system response
  /*! The ReceptionCalibrator implements a technique of single dish
    polarimetric self-calibration.  This class requires a number of
    constraints, which are provided in through the add_observation,
    add_PolnCalibrator, and add_FluxCalibrator methods.
  */
  class ReceptionCalibrator : public Calibrator {
    
  public:
    
    friend class ReceptionCalibratorPlotter;
    
    //! Construct with optional first pulsar archive
    ReceptionCalibrator (const Archive* archive = 0);
    
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
    
    //! Add the specified pulsar observation to the set of constraints
    void add_observation (const Archive* data);
    
    //! Add the specified calibrator observation to the set of constraints
    void add_calibrator (const Archive* data);
    
    //! Add the specified PolnCalibrator observation to the set of constraints
    void add_PolnCalibrator (const PolnCalibrator* polncal);
    
    //! Add the specified FluxCalibrator observation to the set of constraints
    void add_FluxCalibrator (const FluxCalibrator* fluxcal);
    
    //! Solve equation for each frequency
    void solve (int only_ichan = -1);
    
    //! Get the status of the model
    bool get_solved () const;
    
    //! Calibrate the polarization of the given archive
    virtual void calibrate (Archive* archive);
    
    //! Pre-calibrate the polarization of the given archive
    virtual void precalibrate (Archive* archive);
    
  protected:
    
    //! Calibrate the polarization of the given archive
    void calibrate (Archive* archive, bool solve_first);
    
    //! The calibration model as a function of frequency
    vector< Reference::To<StandardModel> > model;
    
    //! Uncalibrated estimate of calibrator polarization
    SourceEstimate calibrator;
    
    //! Uncalibrated estimate of pulsar polarization as a function of phase
    vector<SourceEstimate> pulsar;
    
    //! The calibrators to be loaded during initial_observation
    vector<string> calibrator_filenames;
    
    //! The parallactic angle rotation
    Calibration::Parallactic parallactic;

    //! The time axis
    Calibration::Axis<MJD> time;

    //! Uncalibrated best estimate of the average pulse profile
    Reference::To<const Archive> uncalibrated;

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
      \retval bins the vector to which an new MeasuredState will be appended
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

  private:
    //! Flag set after the solve method has been called
    bool is_fit;

    //! Flag set after the initialize method has been called
    bool is_initialized;

  };

}

#endif
