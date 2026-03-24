//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/CovarianceReport.h

#ifndef __CovarianceReport_H
#define __CovarianceReport_H

#include "Pulsar/ReceptionModelReport.h"

namespace Calibration
{
  //! Print the covariance matrix of the fitted parameters
  class CovarianceReport : public ReceptionModel::Report
  {

  public:

    //! Construct with filename
    CovarianceReport (const std::string& name)
      : ReceptionModel::Report (name) { }

    //! Return a new, copy-constructed clone
    ReceptionModel::Report* clone () const 
    { return new CovarianceReport (*this); }

    //! Write the report
    void report (std::ostream&);
  };
}

#endif

