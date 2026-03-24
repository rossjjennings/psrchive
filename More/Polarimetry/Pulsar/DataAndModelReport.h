//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2023 by Lucas Guillemot and Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/DataAndModelReport.h

#ifndef __DataAndModelReport_H
#define __DataAndModelReport_H

#include "Pulsar/ReceptionModelReport.h"
#include <set>

namespace Calibration
{
  //! Print the data and model values for each pulsar
  class DataAndModelReport : public ReceptionModel::Report
  {

  public:

    //! Construct with filename
    DataAndModelReport (const std::string& name)
      : ReceptionModel::Report (name) { }

    //! Return a new, copy-constructed clone
    ReceptionModel::Report* clone () const 
    { return new DataAndModelReport (*this); }

    //! Write the report for each pulsar
    void report ();

    //! Write the report for the current pulsar
    void report (std::ostream&);

    //! Return the names of all sources in the data set
    std::set<std::string> get_source_names ();

  private:
    std::string current_source_name;
  };

}

#endif

