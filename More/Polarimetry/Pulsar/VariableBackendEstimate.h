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
    typedef MEAL::ProductRule<MEAL::Complex2> Product;
    
    //! The response for pulsar observations
    Reference::To< Product > psr_response;

    //! The response for calibrator observations
    Reference::To< Product > cal_response;

    //! The VariableBackend component of the response
    Reference::To<VariableBackend> variable_backend;

    //! Instrumental gain experienced only by the calibrator
    /*! If the instrumental gain must be held constant for the pulsar
      observations (e.g. because all observations have been normalized
      using the invariant interval), then it is necessary to include
      an additional free parameter for the calibrator observations. */
    Reference::To<VariableGain> cal_gain;
    
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

    //! Get the response for pulsar observations
    Product* get_psr_response () { return psr_response; }

    //! Get the response for calibrator observations
    Product* get_cal_response () { return cal_response; }
    
    //! Set true when the pulsar Stokes parameters have been normalized
    void set_psr_constant_gain (bool = true);

    //! Set true when the cal signal is coupled after the feed
    void set_cal_backend_only (bool = true);
    
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

