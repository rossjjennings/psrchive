//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/tempo/Pulsar/ParametersDM.h

#ifndef __PulsarParametersDM_h
#define __PulsarParametersDM_h

#include "Pulsar/Parameters.h"
#include "MJD.h"

namespace Pulsar {

  //! Computes the dispersion measure for a given epoch from a set of DM coefficients
  class ParametersDM : public Reference::Able {

    //! Reference to the Parameters containing the DM information
    Reference::To<const Pulsar::Parameters> parameters;

    //! The epoch at which the DM is defined
    MJD dm_epoch;

    //! The DM parsed from the Parameters instance
    double dm = 0.0;

    //! The DM coefficient values from the Parameters instance (if any)
    std::vector<double> dm_coeffs;

    //! The interpretation of the DM series (e.g., POLY or TAYLOR)
    bool is_taylor_series = true;

  public:

    //! Set the Parameters instance from which to read DM values
    void set_parameters (const Pulsar::Parameters* params);

    //! Get the epoch at which the DM is defined
    MJD get_dm_epoch();

    //! Calculate the DM at the reference epoch
    double get_dm ();
    
    //! Get the DM coefficients from the Parameters instance
    void get_dm_coeffs (std::vector<double>& dm_coeffs);

    //! Calculate the DM at the given epoch
    double get_dm (const MJD& epoch);
  };

}

#endif
