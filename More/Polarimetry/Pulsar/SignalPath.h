//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/SignalPath.h

#ifndef __Calibration_SignalPath_H
#define __Calibration_SignalPath_H

// Reception Model and its management
#include "Pulsar/ReceptionModel.h"
#include "Pulsar/ReceptionModelSolver.h"

#include "Pulsar/CalibratorType.h"
#include "Pulsar/ConvertMJD.h"

#include "MEAL/ProductRule.h"
#include "MEAL/ChainRule.h"
#include "MEAL/Univariate.h"
#include "MEAL/Gain.h"
#include "MEAL/Axis.h"
#include "MEAL/Real4.h"

namespace Calibration
{
  //! Manages multiple signal path transformations in a reception model

  /*! As in Table 1 of van Straten (2004), different types of
    observations are subjected to different polarimetric
    transformations */

  class VariableBackendEstimate;
  class BackendEstimate;
  
  class SignalPath : public Reference::Able
  {
  public:

    //! Verbosity flag
    static bool verbose;

    //! Default constructor
    SignalPath (Pulsar::Calibrator::Type*);

    //! Copy the state of another instance
    void copy (SignalPath*);

    //! Set the response transformation
    void set_response (MEAL::Complex2*);

    //! Allow the specified response parameter to vary as a function of time
    void set_response_variation (unsigned iparam,
				 MEAL::Univariate<MEAL::Scalar>*);

    //! Get the specified response parameter temporal variation function
    const MEAL::Univariate<MEAL::Scalar>*
    get_response_variation (unsigned iparam) const;

    //! Include an impurity transformation
    void set_impurity (MEAL::Real4*);

    //! Set the transformation to the receptor basis
    void set_basis (MEAL::Complex2*);

    bool has_basis () const { return basis; }
    MEAL::Complex2* get_basis () { return basis; }
    const MEAL::Complex2* get_basis () const { return basis; }

    //! Set true when the pulsar Stokes parameters have been normalized
    void set_constant_pulsar_gain (bool = true);

    //! Manages the pulsar signal path
    class PulsarPath;

    //! Manages the reference source (noise diode) signal path
    class PolnCalPath;

    //! Set the transformation to be cloned for each calibrator
    void set_foreach_calibrator (const MEAL::Complex2*);

    //! Set gain to the univariate function of time
    void set_gain (MEAL::Univariate<MEAL::Scalar>*);

    //! Set differential gain to the univariate function of time
    void set_diff_gain (MEAL::Univariate<MEAL::Scalar>*);

    //! Set differential phase to the univariate function of time
    void set_diff_phase (MEAL::Univariate<MEAL::Scalar>*);

    //! Get the gain function of time
    const MEAL::Scalar* get_gain () const;

    //! Get the differential gain function of time
    const MEAL::Scalar* get_diff_gain () const;

    //! Get the differential phase function of time
    const MEAL::Scalar* get_diff_phase () const;

    //! Add a step if any of the above functions require it
    void add_calibrator_epoch (const MJD&);

    //! Add a step to the gain variations
    void add_gain_step (const MJD&);

    //! Add a step to the differential gain variations
    void add_diff_gain_step (const MJD&);

    //! Add a step to the differential phase variations
    void add_diff_phase_step (const MJD&);

    //! Record the epochs of observations
    void add_observation_epoch (const MJD&);

    //! Set the reference epoch
    void set_reference_epoch (const MJD& epoch);

    //! Update the relevant estimate
    void update ();

    //! Allow the absolute gain to vary as a free parameter
    void fit_gain (bool flag);

    //! Set the receptor ellipticities to a single value
    void equal_ellipticities ();

    //! Attempt to reduce the number of degrees of freedom in the model
    bool reduce_nfree ();

    //! Check all relevant constraints
    void check_constraints ();

    //! Solve the measurement equation
    void solve ();

    //! Add a new signal path for the poln calibrator observations
    void add_polncal_backend ();

    //! Fix the rotation about the line of sight
    void fix_orientation ();

    //! Get the index for the signal path experienced by the reference source
    unsigned get_polncal_path () const { return ReferenceCalibrator_path; }

    //! Get the index for the signal path experienced by the pulsar
    unsigned get_pulsar_path () const { return Pulsar_path; }

    //! Integrate a calibrator solution
    void integrate_calibrator (const MJD& epoch,
			       const MEAL::Complex2* xform);

    //! Get the measurement equation
    Calibration::ReceptionModel* get_equation ();
    const Calibration::ReceptionModel* get_equation () const;

    //! Set the measurement equation
    void set_equation (Calibration::ReceptionModel*);

    //! Set the algorithm used to solve the measurement equation
    void set_solver (Calibration::ReceptionModel::Solver*);

    //! Copy the parameters for the signal path experienced by the pulsar
    void copy_transformation (const MEAL::Complex2*);

    //! Get the instrumental transformation
    const MEAL::Complex2* get_transformation () const;

    //! Get the instrumental transformation
    MEAL::Complex2* get_transformation ();

    //! Add a transformation to the ReceptionModel
    void add_transformation (MEAL::Complex2*);

    //! Get the full signal path experienced by the pulsar
    const MEAL::Complex2* get_pulsar_transformation () const;

    //! Get the covariance vector at the specified epoch
    void get_covariance( std::vector<double>& covar, const MJD& epoch );

    //! The time axis
    MEAL::Axis<MJD> time;

    //! Deactivate time variations and set the Instrument to the given epoch
    void disengage_time_variations (const MJD& epoch);

    //! Activate time varations
    void engage_time_variations ();

    //! Set whether steps occur before or after cals
    void set_step_after_cal (bool flag = true);

    //! e.g. true if noise diode illuminates feed; false if coupled after OMT
    void set_refcal_through_frontend (bool flag = true);

    void set_valid (bool f, const char* reason = 0);
    bool get_valid () const { return valid; }

    MEAL::Axis< Jones<double> >& get_projection () { return projection; }

 protected:

    //! validity flag
    bool valid;

    //! The measurement equation
    Reference::To< Calibration::ReceptionModel > equation;

    //! The algorithm used to solve the measurement equation
    Reference::To< Calibration::ReceptionModel::Solver > solver;

    //! The instrumental response to be modelled
    
    /*! All signal paths include this transformation, which is the
      response to be modeled and output as a solution.  All other
      transformations deal with variations as a function of time,
      variations between signals of different types, etc. */
    
    Reference::To< MEAL::Complex2 > response;

    //! The basis correction computed by the BasisCorrection class
    Reference::To<MEAL::Complex2> basis;

    //! The instrumental response multiplied by the basis
    Reference::To< MEAL::ProductRule<MEAL::Complex2> > instrument;

    //! The known transformations from the sky to the receptors
    /*! The axis class is used to set this constant for each observation */
    MEAL::Axis< Jones<double> > projection;

    //! The instrumental response experienced by the calibrator

    /*! When the gain component of the response must be held constant for
      the pulsar observations (e.g. because all observations have been
      normalized by the invariant interval) it is necessary to include
      an additional free parameter for the calibrator observations. */

    Reference::To< MEAL::ChainRule<MEAL::Complex2> > pcal_gain_chain;
    Reference::To< MEAL::Gain<MEAL::Complex2> > pcal_gain;

    //! ChainRule used to model response parameter variations
    Reference::To< MEAL::ChainRule<MEAL::Complex2> > response_chain;

    //! Temporal variation of response parameters
    typedef Reference::To<MEAL::Univariate<MEAL::Scalar> > ScalarReference;
    std::map< unsigned, ScalarReference > response_variation;

    //! Used to convert MJD to double
    Calibration::ConvertMJD convert;

    //! Estimate of the backend component of response
    Reference::To<BackendEstimate> backend_estimate;
    
    //! The set of instrumental backend transformations for each epoch
    /*! This is the new way to handle jumps in the instrumental response
      that apply to both calibrator and pulsar observations. */
    std::vector< Reference::To< VariableBackendEstimate > > backends;

    //! Transformation cloned for each calibrator observation
    Reference::To< const MEAL::Complex2 > foreach_pcal;
    Reference::To< const MEAL::Complex2 > foreach_fcal;

    //! The signal path experienced by the pulsar
    Reference::To< MEAL::ProductRule<MEAL::Complex2> > pulsar_path;


    //! The Mueller transformation
    Reference::To< MEAL::Real4 > impurity;



    void integrate_parameter (MEAL::Scalar* function, double value);

    void update_parameter (MEAL::Scalar* function, double value);

    void compute_covariance( unsigned index, 
			     std::vector< std::vector<double> >& covar,
			     std::vector<unsigned>& function_imap, 
			     MEAL::Scalar* function );

    //! Set the minimum step if Scalar is a Steps
    void set_min_step (MEAL::Scalar* function, double minimum);

    //! Offset the steps if Scalar is a Steps
    void offset_steps (MEAL::Scalar* function, double offset);

    //! Insert a step into the instrumental response as a function of time
    void add_step (const MJD& mjd);

    //! Allow specified parameter to vary freely in step that spans mjd
    void set_free (unsigned iparam, const MJD& mjd);

    //! Remove the last step if there is no data there
    void fix_last_step (MEAL::Scalar* function);

    bool time_variations_engaged;
    bool step_after_cal;
    bool constant_pulsar_gain;
    bool refcal_through_frontend;
    
    // ////////////////////////////////////////////////////////////////////
    //
    //! The signal path of the ReferenceCalibrator source
    unsigned ReferenceCalibrator_path;

    //! The signal path of the Pulsar phase bin sources
    unsigned Pulsar_path;

    MJD min_epoch, max_epoch;

  private:

    //! Parameterization of the instrument
    Reference::To<Pulsar::Calibrator::Type> type;

    //! built flag
    bool built;

    //! build method
    void build ();

    //! build called from const method
    void const_build () const;

  };
  



}

#endif
