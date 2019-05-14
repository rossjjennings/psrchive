//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/SystemCalibrator.h

#ifndef __Pulsar_SystemCalibrator_H
#define __Pulsar_SystemCalibrator_H

#include "Pulsar/PolnCalibrator.h"

#include "Pulsar/ReceptionModelSolver.h"
#include "Pulsar/SignalPath.h"

#include "Pulsar/CoherencyMeasurementSet.h"
#include "Pulsar/SourceEstimate.h"

#include "BatchQueue.h"

namespace Pulsar
{
  class ReferenceCalibrator;
  class FluxCalibrator;
  class CalibratorStokes;

  //! PolnCalibrator with estimated calibrator Stokes parameters
  /*! The SystemCalibrator is the virtual base class of polarization
    calibrators that determine both the instrumental response and the
    input Stokes parameters of the reference signal.
  */
  class SystemCalibrator : public PolnCalibrator
  {

  public:

    typedef Calibration::ReceptionModel::Solver Solver;

    //! Construct with optional processed calibrator Archive
    SystemCalibrator (Archive* archive = 0);

    //! Copy constructor
    SystemCalibrator (const SystemCalibrator&);

    //! Destructor
    virtual ~SystemCalibrator ();

    //! Return the Calibrator information
    Calibrator::Info* get_Info () const;

    //! Return the reference epoch of the calibration experiment
    MJD get_epoch () const;

    //! Get the number of frequency channels
    unsigned get_nchan () const;

    //! Get the number of data points in the given frequency channel
    unsigned get_ndata (unsigned ichan) const;

    //! Get the total number of input polarization states (pulsar and cal)
    virtual unsigned get_nstate () const;
    
    //! Get the number of pulsar polarization states in the model
    virtual unsigned get_nstate_pulsar () const;

    //! Return true if the state index is a pulsar
    virtual unsigned get_state_is_pulsar (unsigned istate) const;

    //! Return true if calibrator (e.g. noise diode) data are incorporated
    virtual bool has_cal () const { return calibrator_estimate.size(); }

    //! Retern a new plot information interface for the specified pulsar state
    virtual Calibrator::Info* new_info_pulsar (unsigned istate) const;

    //! True if noise diode illuminates feed; false if coupled after OMT
    void set_refcal_through_frontend (bool flag = true);
    
    //! Set the flux calibrator solution used to estimate calibrator Stokes
    void set_flux_calibrator (const FluxCalibrator* fluxcal);

    //! Set the calibrator observations to be loaded after first pulsar
    void set_calibrators (const std::vector<std::string>& filenames);
    
    //! Set the calibrator
    virtual void set_calibrator (const Archive*);

    //! Set the response (pure Jones) transformation
    virtual void set_response( MEAL::Complex2* );

    //! Set the impurity transformation
    virtual void set_impurity( MEAL::Real4* );

    //! Set the time variation of absolute gain
    virtual void set_gain( MEAL::Univariate<MEAL::Scalar>* );

    //! Set the time variation of differential gain
    virtual void set_diff_gain( MEAL::Univariate<MEAL::Scalar>* );

    //! Set the time variation of differential phase
    virtual void set_diff_phase( MEAL::Univariate<MEAL::Scalar>* );

    //! Add a step to the gain variations
    virtual void add_gain_step (const MJD&);

    //! Add a step to the differential gain variations
    virtual void add_diff_gain_step (const MJD&);

    //! Add a step to the differential phase variations
    virtual void add_diff_phase_step (const MJD&);

    //! Set the transformation to be cloned for each calibrator
    virtual void set_foreach_calibrator( const MEAL::Complex2* );

    //! Prepare the data for inclusion in the model
    virtual void preprocess (Archive* data);

    //! Add the observation to the set of constraints
    virtual void add_observation (const Archive* data);

    //! Add the pulsar observation to the set of constraints
    virtual void add_pulsar (const Archive* data);

    //! Add the calibrator observation to the set of constraints
    virtual void add_calibrator (const Archive* data);

    //! Add the ReferenceCalibrator observation to the set of constraints
    virtual void add_calibrator (const ReferenceCalibrator* polncal);

    //! Add an observational epoch
    virtual void add_epoch (const MJD& epoch);
   
    //! Get the epoch of the first observation
    MJD get_start_epoch () const { return start_epoch; }

    //! Get the epoch of the last observation
    MJD get_end_epoch () const { return end_epoch; }

    //! Set the number of channels that may be simultaneously solved
    virtual void set_nthread (unsigned nthread);

    //! Set the measurement equation configuration options
    virtual void set_equation_configuration (const std::vector<std::string>&);

    //! Return true if least squares minimization solvers are available
    virtual bool has_solver () const;

    //! Return the solver for the specified channel
    virtual const Solver* get_solver (unsigned ichan) const;

    //! Set the algorithm used to solve the measurement equation
    virtual void set_solver (Solver*);

    //! Get the algorithm used to solve the measurement equation
    virtual Solver* get_solver ();
    
    //! Set the reduced chisq above which the solution will be retried
    virtual void set_retry_reduced_chisq (float);

    //! Set the reduced chisq above which a solution will be flagged invalid
    virtual void set_invalid_reduced_chisq (float);

    //! Report on the projection correction used in add_pulsar method
    virtual void set_report_projection (bool);

    //! Report on the initial state of the model before fitting
    virtual void set_report_initial_state (bool flag = true);

    //! Report on the data included as constraints before fitting
    virtual void set_report_input_data (bool flag = true);

    //! Set the threshold used to reject outliers when computing levels
    void set_outlier_threshold (float f) { outlier_threshold = f; }

    //! Get the threshold used to reject outliers when computing levels
    float get_outlier_threshold () const { return outlier_threshold; }

    //! Solve equation for each frequency
    virtual void solve ();
    
    //! Get the status of the model
    virtual bool get_solved () const;

    //! Returns true if at least one channel returns get_valid == true
    virtual bool has_valid () const;

    //! Get the reduced chisq of the best fit in the specified channel
    float get_reduced_chisq (unsigned ichan) const;

    //! Pre-calibrate the polarization of the given archive
    virtual void precalibrate (Archive* archive);

    //! Return the transformation to be used for precalibration
    virtual MEAL::Complex2* get_transformation (const Archive* data,
						unsigned isub, unsigned ichan);

    //! Return a Calibrator::new_solution with a CalibratorStokesExtension
    virtual Archive* new_solution (const std::string& archive_class) const;

    //! Return the CalibratorStokesExtension
    virtual const CalibratorStokes* get_CalibratorStokes () const;

    //! Return the SignalPath for the specified channel
    virtual const Calibration::SignalPath* get_model (unsigned ichan) const;

    //! Solution unloading policy
    class Unloader;

  protected:

    friend class SystemCalibratorPlotter;
    friend class MatrixTemplateMatching;

    //! Prepare the model
    virtual void prepare (const Archive* data);

    //! Initialize the PolnCalibration::transformation attribute
    virtual void calculate_transformation ();

    //! Create the model array
    void create_model ();

    //! The standard calibration model as a function of frequency
    Reference::Vector<Calibration::SignalPath> model;

    //! Meausrement equation configuration options
    std::vector<std::string> equation_configuration;

    //! The algorithm used to solve the measurement equation
    Reference::To<Solver> solver;

    //! The FluxCalibrator solution
    Reference::To<const FluxCalibrator> flux_calibrator;
    
    //! The CalibratorStokesExtension of the Archive passed during construction
    mutable Reference::To<const CalibratorStokes> calibrator_stokes;

    //! Response transformation
    Reference::To< MEAL::Complex2 > response;

    //! Impurity transformation
    Reference::To< MEAL::Real4 > impurity;

    //! Time variation of absolute gain
    Reference::To< MEAL::Univariate<MEAL::Scalar> > gain_variation;

    //! Time variation of differential gain
    Reference::To< MEAL::Univariate<MEAL::Scalar> > diff_gain_variation;

    //! Time variation of differential phase
    Reference::To< MEAL::Univariate<MEAL::Scalar> > diff_phase_variation;

    std::vector<MJD> gain_steps;
    std::vector<MJD> diff_gain_steps;
    std::vector<MJD> diff_phase_steps;

    //! Transformation cloned for each calibrator observation
    Reference::To< const MEAL::Complex2 > foreach_calibrator;

    //! Initialize the SignalPath of the specified channel
    virtual void init_model (unsigned ichan);

    //! Initialize a vector of SourceEstimate instances
    virtual void init_estimates ( std::vector<Calibration::SourceEstimate>&,
				  unsigned ibin = 0 );

    //! Prepare any calibrator estimates
    virtual void prepare_calibrator_estimate (Signal::Source);

    //! Create the calibrator estimate
    virtual void create_calibrator_estimate ();

    virtual void submit_calibrator_data (Calibration::CoherencyMeasurementSet&,
					 const Calibration::SourceObservation&);

    virtual void integrate_calibrator_data (const Jones< Estimate<double> >&,
					    const Calibration::SourceObservation&);

    virtual void integrate_calibrator_solution (Signal::Source source,
						unsigned ichan,
						const MEAL::Complex2*);

    //! Load any postponed calibrators and those set by set_calibrators
    virtual void load_calibrators ();

    //! Ensure that the pulsar observation can be added to the data set
    virtual void match (const Archive*);

    //! Add the data from the specified sub-integration
    virtual void add_pulsar (const Archive* data, unsigned isub);

    //! Derived types must define how pulsar data are incorporated
    virtual void add_pulsar (Calibration::CoherencyMeasurementSet&,
			     const Integration*, unsigned ichan) = 0;

    //! The calibrators to be loaded after first pulsar observation
    std::vector<std::string> calibrator_filenames;

    //! Uncalibrated estimate of calibrator polarization
    std::vector<Calibration::SourceEstimate> calibrator_estimate;
    
    //! Epoch of the first observation
    MJD start_epoch;

    //! Epoch of the last observation
    MJD end_epoch;

    //! Include a correction for Faraday rotation in the ISM
    bool correct_interstellar_Faraday_rotation;
    
    //! True if noise diode illuminates feed; false if coupled after OMT
    bool refcal_through_frontend;

    //! Set the initial guess in solve_prepare
    bool set_initial_guess;

    //! Ensure that first guess of calibrator Stokes parameters is physical
    bool guess_physical_calibrator_stokes;
    
    //! The maximum reduced chisq before another fit will be attempted
    float retry_chisq;

    //! The maximum reduced chisq before a fit will be flagged invalid
    float invalid_chisq;

    //! Report on the projection transformation used in add_pulsar
    bool report_projection;

    //! Report the initial state of model before fitting
    bool report_initial_state;

    //! Report on the data included as constraints
    bool report_input_data;

    //! Threshold used to reject outliers when computing levels
    double outlier_threshold;
    
    //! Prepare the measurement equations for fitting
    virtual void solve_prepare ();

    //! Configure a measurement equation 
    void configure ( MEAL::Function* equation );

    //! Prepare to export the solution in current state; e.g. for plotting
    virtual void export_prepare () const = 0;

    //! Controls the number of channels that may be simultaneously solved
    BatchQueue queue;

    //! Get the state of the prepared flag
    bool get_prepared () const;

    //! Return the transformation for the specified channel
    Solver* get_solver (unsigned ichan);

    //! Solve the specified channel after copying a good solution from another
    void resolve (unsigned ichan);

    //! ensure that ichan < model.size()
    void check_ichan (const char* name, unsigned ichan) const;

    unsigned get_data_fail;
    unsigned get_data_call;

    //! A previous solution, if availabe
    Reference::To<const PolnCalibrator> previous;
    Reference::To<const CalibratorStokes> previous_cal;

    //! Flag set after the first pulsar observation is added
    bool has_pulsar;

  private:

    std::vector<bool> epoch_added;

    Jones<double> invert_basis;

    //! Flag set after the solve method has been called
    bool is_solved;

    //! Flag set after the solve_prepare method has been called
    bool is_prepared;

  };

}

#endif

