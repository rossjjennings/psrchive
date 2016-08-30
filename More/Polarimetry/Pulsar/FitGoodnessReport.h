//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/FitGoodnessReport.h

#ifndef __FitGoodnessReport_H
#define __FitGoodnessReport_H

#include "Pulsar/ReceptionModelReport.h"

namespace Calibration
{
  //! Report on the goodness-of-fit for each source
  class FitGoodnessReport : public ReceptionModel::Report
  {

  public:

    //! Construct with filename
    FitGoodnessReport (const std::string& name)
      : ReceptionModel::Report (name) { }

    //! Return a new, copy-constructed clone
    ReceptionModel::Report* clone () const 
    { return new FitGoodnessReport (*this); }

    //! Write the report
    void report (std::ostream&);

  };

}

#endif

