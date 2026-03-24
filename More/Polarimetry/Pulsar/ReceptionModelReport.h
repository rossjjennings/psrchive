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
  //! Base class of objects that report on properties of the measurement equation 
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
    void set_model (ReceptionModel* _model) { model.set(_model); }

    //! Set the name of the file to which the report will be written
    void set_filename (const std::string& _name) { filename = _name; }
    const std::string& get_filename () const { return filename; }

    //! Set the root path to which reports will be written
    void set_path (const std::string& _name) { path = _name; }
    const std::string& get_path () const { return path; }

    //! Create the reports directory and return the constructed path name
    std::string make_reports_path (const std::string& subdir = "");

    //! Write the report
    virtual void report ();

  protected:

    //! Path to which reports are written
    std::string path;

    //! Filename to which the report is written
    std::string filename;

    //! The measurement equation for which report is written
    Reference::To<ReceptionModel, false> model;
  };

}

#endif

