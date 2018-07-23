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
#include "Pulsar/MeanPolar.h"
#include "Pulsar/MeanSingleAxis.h"
#include "Pulsar/ConvertMJD.h"

#include "MEAL/ProductRule.h"
#include "MEAL/ChainRule.h"
#include "MEAL/Polynomial.h"
#include "MEAL/Gain.h"
#include "MEAL/Axis.h"
#include "MEAL/Real4.h"

namespace Calibration
{
  //! Manages multiple signal path transformations in a reception model

  /*! As in Table 1 of van Straten (2004), different types of
    observations will experience different polarimetric
    transformations */

  class SignalPath : public Reference::Able
  {
  public:

    //! Verbosity flag
    static bool verbose;

    //! Default constructor
    SignalPath (Pulsar::Calibrator::Type*);

    //! Set the response transformation
    void set_response (MEAL::Complex2*);

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

    //! Set gain to the univariate function of time
    const MEAL::Scalar* get_gain () const;

    //! Set differential gain to the univariate function of time
    const MEAL::Scalar* get_diff_gain () const;

    //! Set differential phase to the univariate function of time
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

    //! Work around the fact that no reference calibrators are available
    void no_reference_calibrators (bool fit_gain);

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
    void integrate_calibrator (const MEAL::Complex2* xform);

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

    //! The known transformations from the sky to the receptors
    MEAL::Axis< Jones<double> > projection;

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

 protected:

    //! validity flag
    bool valid;

    //! ReceptionModel
    Reference::To< Calibration::ReceptionModel > equation;

    //! The algorithm used to solve the measurement equation
    Reference::To< Calibration::ReceptionModel::Solver > solver;

    //! Used to convert MJD to double
    Calibration::ConvertMJD convert;

    //! The signal path experienced by the calibrator
    Reference::To< MEAL::ChainRule<MEAL::Complex2> > pcal_gain_chain;
    Reference::To< MEAL::Gain<MEAL::Complex2> > pcal_gain;

    //! Transformation cloned for each calibrator observation
    Reference::To< const MEAL::Complex2 > foreach_pcal;
    Reference::To< const MEAL::Complex2 > foreach_fcal;

    //! The signal path experienced by the pulsar
    Reference::To< MEAL::ProductRule<MEAL::Complex2> > pulsar_path;

    //! The response multiplied by the basis
    Reference::To< MEAL::ProductRule<MEAL::Complex2> > instrument;

    //! The Mueller transformation
    Reference::To< MEAL::Real4 > impurity;

    //! The instrumental response
    Reference::To< MEAL::Complex2 > response;

    //! The best estimate of the backend
    Calibration::MeanSingleAxis backend_estimate;

    //! The best estimate of the polar model
    Calibration::MeanPolar polar_estimate;

    //! The basis transformation
    Reference::To<MEAL::Complex2> basis;

    //! The backend variation transformations
    Reference::To< MEAL::Scalar > gain;
    Reference::To< MEAL::Scalar > diff_gain;
    Reference::To< MEAL::Scalar > diff_phase;

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

    //! Add a step if Scalar is a Steps
    void add_step (MEAL::Scalar* function, double step);

    //! Insert a step into the Scalar, which must be a Steps
    void add_step (MEAL::Scalar* function, const MJD& mjd);

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
