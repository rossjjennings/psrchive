//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/ModelParametersReport.h

#ifndef __ModelParametersReport_H
#define __ModelParametersReport_H

#include "Pulsar/ReceptionModelReport.h"

namespace Calibration
{
  //! Report on the reception model paramters
  class ModelParametersReport : public ReceptionModel::Report
  {

  public:

    //! Construct with filename
    ModelParametersReport (const std::string& name)
      : ReceptionModel::Report (name) { }

    //! Return a new, copy-constructed clone
    ReceptionModel::Report* clone () const 
    { return new ModelParametersReport (*this); }

    //! Write the report
    void report (std::ostream&);

  };

}

#endif

