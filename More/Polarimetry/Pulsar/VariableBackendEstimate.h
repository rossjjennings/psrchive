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
#include "MEAL/Univariate.h"

#include <map>

namespace Calibration
{
  class IndexedProduct : public MEAL::ProductRule<MEAL::Complex2>
  {
    int index;
    
  public:
    IndexedProduct () { index = -1; }

    bool has_index () const { return index >= 0; }
    void set_index (unsigned _index) { index = _index; }
    unsigned get_index () const { return index; }

  };
  
  //! Manages a variable backend and its best estimate
  class VariableBackendEstimate : public BackendEstimate
  {
    //! The response for pulsar observations
    Reference::To< IndexedProduct > psr_response;
    
    //! The response for calibrator observations
    Reference::To< IndexedProduct > cal_response;

    //! The VariableBackend component of the response
    Reference::To<VariableBackend> variable_backend;

    //! The calibrator is transformed by only the variable_backend
    bool cal_backend_only;
    
    //! Instrumental gain experienced only by the calibrator
    /*! If the instrumental gain must be held constant for the pulsar
      observations (e.g. because all observations have been normalized
      using the invariant interval), then it is necessary to include
      an additional free parameter for the calibrator observations. */
    Reference::To<VariableGain> cal_gain;
    
    //! The backend variation transformations
    Reference::To< MEAL::Scalar > gain_variation;
    Reference::To< MEAL::Scalar > diff_gain_variation;
    Reference::To< MEAL::Scalar > diff_phase_variation;

    /*! Mapping between variation transformation indeces
      and measurement equation indeces */
    std::vector< unsigned > gain_imap;
    std::vector< unsigned > diff_gain_imap;
    std::vector< unsigned > diff_phase_imap;
    std::vector< unsigned > backend_imap;
    
  public:

    //! Construct using the supplied response
    VariableBackendEstimate (MEAL::Complex2* response = 0);

    //! Set the response that contains the backend
    void set_response (MEAL::Complex2* xform);

    //! Multiply psr_response by xform and cal_response by backend
    void add_model (MEAL::Complex2* xform);

    //! Get the response for pulsar observations
    IndexedProduct* get_psr_response () { return psr_response; }

    //! Get the response for calibrator observations
    IndexedProduct* get_cal_response () { return cal_response; }

    //! Get the backend component
    SingleAxis* get_backend () { return variable_backend->get_backend(); }
    
    //! Set true when the pulsar Stokes parameters have been normalized
    void set_psr_constant_gain (bool = true);

    //! Set true when the cal signal is coupled after the feed
    void set_cal_backend_only (bool = true);
    
    void set_gain_variation (MEAL::Univariate<MEAL::Scalar>*);
    void set_diff_gain_variation (MEAL::Univariate<MEAL::Scalar>*);
    void set_diff_phase_variation (MEAL::Univariate<MEAL::Scalar>*);

    //! Used to convert MJD to double
    Calibration::ConvertMJD convert;
    
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

