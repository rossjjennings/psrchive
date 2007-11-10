//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/StandardModel.h,v $
   $Revision: 1.13 $
   $Date: 2007/11/10 12:14:50 $
   $Author: straten $ */

#ifndef __Calibration_StandardModel_H
#define __Calibration_StandardModel_H

// Reception Model and its management
#include "Pulsar/ReceptionModel.h"

#include "Pulsar/MeanPolar.h"
#include "Pulsar/MeanSingleAxis.h"
#include "Pulsar/Instrument.h"
#include "Pulsar/Parallactic.h"
#include "Pulsar/ConvertMJD.h"

#include "MEAL/Polynomial.h"
#include "MEAL/Polar.h"
#include "MEAL/Axis.h"

namespace Pulsar {
  class ReceptionCalibrator;
}

namespace Calibration {

  //! Stores the various elements related to the calibration model
  class StandardModel : public Reference::Able {

  public:
    
    //! Default constructor
    StandardModel (bool britton = true);

    //! Set the transformation from the feed to the receptor basis
    void set_feed_transformation (MEAL::Complex2* xform);

    //! Set the transformation from the platform to the feed basis
    void set_platform_transformation (MEAL::Complex2* xform);

    //! Set gain to the univariate function of time
    void set_gain (MEAL::Univariate<MEAL::Scalar>*);

    //! Set differential gain to the univariate function of time
    void set_diff_gain (MEAL::Univariate<MEAL::Scalar>*);

    //! Set differential phase to the univariate function of time
    void set_diff_phase (MEAL::Univariate<MEAL::Scalar>*);

    //! Add a step if any of the above functions require it
    void add_calibrator_epoch (const MJD&);

    //! Record the epochs of observations
    void add_observation_epoch (const MJD&);

    //! Set the reference epoch
    void set_reference_epoch (const MJD& epoch);

    //! Update the relevant estimate
    void update ();

    //! Check all relevant constraints
    void check_constraints ();

    //! Solve the measurement equation
    void solve ();

    //! Add a new signal path for the poln calibrator observations
    void add_polncal_backend ();

    //! Add a new signal path for the flux calibrator observations
    void add_fluxcal_backend ();

    //! Fix the rotation about the line of sight
    void fix_orientation ();

    //! Get the index for the signal path experienced by the flux calibrator
    unsigned get_fluxcal_path () const { return FluxCalibrator_path; }

    //! Get the index for the signal path experienced by the reference source
    unsigned get_polncal_path () const { return ReferenceCalibrator_path; }

    //! Get the index for the signal path experienced by the pulsar
    unsigned get_pulsar_path () const { return Pulsar_path; }

    //! Integrate a calibrator solution
    void integrate_calibrator (const MEAL::Complex2* xform, bool fluxcal);

    //! Get the measurement equation solver
    Calibration::ReceptionModel* get_equation ();

    //! Copy the parameters for the signal path experienced by the pulsar
    void copy_transformation (const MEAL::Complex2*);

    //! Get the instrumental transformation
    const MEAL::Complex2* get_transformation () const;

    //! Get the instrumental transformation
    MEAL::Complex2* get_transformation ();

    //! Get the full signal path experienced by the pulsar
    const MEAL::Complex2* get_pulsar_transformation () const;

    //! Get the covariance vector at the specified epoch
    void get_covariance( std::vector<double>& covar, const MJD& epoch );

    //! The parallactic angle rotation
    Calibration::Parallactic parallactic;

    //! The time axis
    MEAL::Axis<MJD> time;

    //! validity flag
    bool valid;

 protected:

    //! ReceptionModel
    Reference::To< Calibration::ReceptionModel > equation;

    //! Used to convert MJD to double
    Calibration::ConvertMJD convert;

    //! The signal path experienced by the calibrator
    Reference::To< MEAL::ProductRule<MEAL::Complex2> > pcal_path;

    //! The signal path experienced by the pulsar
    Reference::To< MEAL::ProductRule<MEAL::Complex2> > pulsar_path;

    //! The instrumental model and any additional transformations
    Reference::To< MEAL::ProductRule<MEAL::Complex2> > instrument;

    // ////////////////////////////////////////////////////////////////////
    //
    //! Polar decomposition of instrumental response (Hamaker)
    Reference::To< MEAL::Polar > polar;

    //! The best estimate of the polar model
    Calibration::MeanPolar polar_estimate;

    Reference::To< MEAL::Scalar > gain;
    Reference::To< MEAL::Scalar > diff_gain;
    Reference::To< MEAL::Scalar > diff_phase;

    void integrate_parameter (MEAL::Scalar* function, double value);
    void update_parameter (MEAL::Scalar* function, double value);

    //! Remove all time variations and set the Instrument to the given epoch
    void disengage_time_variations (const MJD& epoch);

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

    // ////////////////////////////////////////////////////////////////////
    //
    //! Phenomenological decomposition of instrumental response (Britton)
    Reference::To< Calibration::Instrument > physical;

    //! The best estimate of the physical model
    Calibration::MeanSingleAxis physical_estimate;

    // ////////////////////////////////////////////////////////////////////
    //
    //! Additional backend required for flux calibrator signal path
    Reference::To< Calibration::SingleAxis > fluxcal_backend;

    //! The best estimate of the flux calibration backend
    Calibration::MeanSingleAxis fluxcal_backend_estimate;

    //! The signal path of the FluxCalibrator source
    unsigned FluxCalibrator_path;

    //! The signal path of the ReferenceCalibrator source
    unsigned ReferenceCalibrator_path;

    //! The signal path of the Pulsar phase bin sources
    unsigned Pulsar_path;

    //! The feed transformation
    Reference::To<MEAL::Complex2> feed_transformation;

    //! The platform transformation
    Reference::To<MEAL::Complex2> platform_transformation;

    MJD min_epoch, max_epoch;

  private:

    //! built flag
    bool built;

    //! build method
    void build ();

    //! build called from const method
    void const_build () const;

  };
  
}

#endif
