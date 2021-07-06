//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/VariableBackendEstimate.h

#ifndef __Pulsar_VariableBackendEstimate_H
#define __Pulsar_VariableBackendEstimate_H

#include "Pulsar/BackendEstimate.h"
#include "Pulsar/VariableBackend.h"
#include "Pulsar/VariableGain.h"
#include "Pulsar/ConvertMJD.h"

#include "MEAL/Scalar.h"
#include "MEAL/ChainRule.h"
#include "MEAL/Univariate.h"

#include <map>

namespace Calibration
{
  //! Manages a variable backend and its best estimate
  class VariableBackendEstimate : public BackendEstimate
  {
    Reference::To<VariableBackend> variable_backend;
    Reference::To<VariableGain> pcal_gain;
    
    //! The backend variation transformations
    Reference::To< MEAL::Scalar > gain;
    Reference::To< MEAL::Scalar > diff_gain;
    Reference::To< MEAL::Scalar > diff_phase;

    /*! Mapping between variation transformation indeces
      and measurement equation indeces */
    std::vector< unsigned > gain_imap;
    std::vector< unsigned > diff_gain_imap;
    std::vector< unsigned > diff_phase_imap;

    //! Used to convert MJD to double
    Calibration::ConvertMJD convert;
    
  public:

    //! Might implement a copy constructor
    VariableBackendEstimate (const BackendEstimate* copy = 0);

    //! Set the response that contains the backend
    void set_response (MEAL::Complex2* xform);

    //! Set true when the pulsar Stokes parameters have been normalized
    void set_constant_pulsar_gain (bool = true);

    void set_gain (MEAL::Univariate<MEAL::Scalar>*);
    void set_diff_gain (MEAL::Univariate<MEAL::Scalar>*);
    void set_diff_phase (MEAL::Univariate<MEAL::Scalar>*);

    //! Update the transformation with the current estimate, if possible
    void update ();

    //! Update one of the variable parameters
    void update (MEAL::Scalar* function, double value);

    //! Attempt to reduced the number of degrees of freedom
    /*! Return true if successful */
    bool reduce_nfree ();

    //! Update the reference epoch
    void update_reference_epoch ();

    //!
    void engage_time_variations ();
    void disengage_time_variations ();

    void unmap_variations (std::vector<unsigned>& imap,
			   MEAL::Complex2* composite);
    
    void compute_covariance (std::vector<unsigned>& imap,
			     std::vector< std::vector<double> >& Ctotal);
    

  };
}

#endif

