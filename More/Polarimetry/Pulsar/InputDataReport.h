//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/InputDataReport.h

#ifndef __InputDataReport_H
#define __InputDataReport_H

#include "Pulsar/ReceptionModelReport.h"

namespace Calibration
{
  //! Report on the reception model paramters
  class InputDataReport : public ReceptionModel::Report
  {

  public:

    //! Construct with filename
    InputDataReport (const std::string& name)
      : ReceptionModel::Report (name) { }

    //! Return a new, copy-constructed clone
    ReceptionModel::Report* clone () const 
    { return new InputDataReport (*this); }

    //! Write the report
    void report (std::ostream&);

  };

}

#endif

