//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/StandardModel.h,v $
   $Revision: 1.3 $
   $Date: 2006/04/08 00:01:43 $
   $Author: straten $ */

#ifndef __Calibration_StandardModel_H
#define __Calibration_StandardModel_H

// Reception Model and its management
#include "Calibration/ReceptionModel.h"

#include "Calibration/MeanPolar.h"
#include "Calibration/MeanSingleAxis.h"
#include "Calibration/Instrument.h"
#include "Calibration/Parallactic.h"
#include "Calibration/ConvertMJD.h"

#include "MEAL/Polar.h"
#include "MEAL/Axis.h"

namespace Pulsar {
  class ReceptionCalibrator;
}

namespace Calibration {

  //! Stores the various elements related to the calibration model
  class StandardModel : public Reference::Able {

    // friend class Pulsar::ReceptionCalibrator;

  public:
    
    //! Default constructor
    StandardModel (bool britton = true);

    //! Set the transformation from the feed to the receptor basis
    void set_feed_transformation (MEAL::Complex2* xform);

    //! Set the transformation from the platform to the feed basis
    void set_platform_transformation (MEAL::Complex2* xform);

    //! Set the order of the differential phase polynomial
    void set_differential_phase_order (unsigned order);

    //! Update the relevant estimate
    void update ();

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

    //! Set the signal path experienced by the pulsar
    void set_transformation (const MEAL::Complex2*);

    //! Get the signal path experienced by the pulsar
    const MEAL::Complex2* get_transformation () const;

    //! Get the signal path experienced by the pulsar
    MEAL::Complex2* get_transformation ();

    //! The parallactic angle rotation
    Calibration::Parallactic parallactic;

    //! The time axis
    MEAL::Axis<MJD> time;

    //! Used to convert MJD to double
    Calibration::ConvertMJD convert;

    //! validity flag
    bool valid;

 protected:

    //! ReceptionModel
    Reference::To< Calibration::ReceptionModel > equation;

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
    
    //! The order of the polynomial for differential phase
    unsigned differential_phase_order;

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
