//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/ReceptionModelReport.h

#ifndef __ReceptionModelReport_H
#define __ReceptionModelReport_H

#include "Pulsar/ReceptionModel.h"

namespace Calibration
{
  //! Solve the measurement equation by non-linear least squares minimization
  class ReceptionModel::Report : public Reference::Able
  {

  public:

    //! Return a new, copy-constructed clone of self
    virtual Report* clone () const = 0;

    //! Write the report
    virtual void report (std::ostream&) = 0;

    //! Construct with filename
    Report (const std::string& name) { filename = name; }

    //! Set the measurement equation on which to report
    void set_model (ReceptionModel* _model) { model = _model; }

    //! Set the name of the file to which the report will be written
    void set_filename (const std::string& _name) { filename = _name; }
    const std::string& get_filename () const { return filename; }

    //! Write the report
    void report ();

  protected:

    //! Filename to which the report will be written
    std::string filename;

    //! The measurement equation to be solved
    Reference::To<ReceptionModel, false> model;

  };

}

#endif

