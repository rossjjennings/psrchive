//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Extensions/Pulsar/CalibrationInterpolatorExtension.h

#ifndef __CalibrationInterpolatorExtension_h
#define __CalibrationInterpolatorExtension_h

#include "Pulsar/ArchiveExtension.h"
#include "Pulsar/CalibratorType.h"

namespace Pulsar {

  //! Stores Polarization Calibration Model Interpolator information
  /*! This Archive::Extension class implements the storage of
    Calibration Interpolator data, enabling such a calibrator to be
    constructed from file. */
  class CalibrationInterpolatorExtension : public Archive::Extension
  {   
  public:
    
    //! Default constructor
    CalibrationInterpolatorExtension
    (const char* name = "CalibrationInterpolatorExtension");

    //! Copy constructor
    CalibrationInterpolatorExtension (const CalibrationInterpolatorExtension&);

    //! Operator =
    const CalibrationInterpolatorExtension&
    operator= (const CalibrationInterpolatorExtension&);

    //! Destructor
    ~CalibrationInterpolatorExtension ();

    //! Clone method
    CalibrationInterpolatorExtension* clone () const
    { return new CalibrationInterpolatorExtension( *this ); }

    //! Set the type of the calibrator
    void set_type (const Calibrator::Type* t);// { type = t; }
    //! Get the type of the calibrator
    const Calibrator::Type* get_type () const;// { return type; }

    //! Set the reference epoch of the interpolator
    void set_reference_epoch (const MJD& epoch) { reference_epoch = epoch; }
    //! Get the reference epoch of the interpolator
    MJD get_reference_epoch () const { return reference_epoch; }

    //! Set the minimum epoch of the interpolator
    void set_minimum_epoch (const MJD& epoch) { minimum_epoch = epoch; }
    //! Get the minimum epoch of the interpolator
    MJD get_minimum_epoch () const { return minimum_epoch; }

    //! Set the maximum epoch of the interpolator
    void set_maximum_epoch (const MJD& epoch) { maximum_epoch = epoch; }
    //! Get the maximum epoch of the interpolator
    MJD get_maximum_epoch () const { return maximum_epoch; }

    //! Set the reference frequency of the interpolator
    void set_reference_frequency (double freq) { reference_frequency = freq; }
    //! Get the reference frequency of the interpolator
    double get_reference_frequency () const { return reference_frequency; }

    //! Set the minimum frequency of the interpolator
    void set_minimum_frequency (double freq) { minimum_frequency = freq; }
    //! Get the minimum frequency of the interpolator
    double get_minimum_frequency () const { return minimum_frequency; }

    //! Set the maximum frequency of the interpolator
    void set_maximum_frequency (double freq) { maximum_frequency = freq; }
    //! Get the maximum frequency of the interpolator
    double get_maximum_frequency () const { return maximum_frequency; }

    //! Return a short name
    std::string get_short_name () const { return "pcmint"; }

    //! Text interface to CalibrationInterpolatorExtension extension
    class Interface
      : public TextInterface::To<Pulsar::CalibrationInterpolatorExtension>
    {
    public:
      Interface (CalibrationInterpolatorExtension* = 0);
    };
    
    //! Return a text interfaces that can be used to access this instance
    TextInterface::Parser* get_interface();

    class Parameter : public Reference::Able
    {
    public:
      std::string code;
      unsigned iparam;
      std::string description;
    };

    void add_parameter (Parameter* p) { parameter.push_back(p); }
    unsigned get_nparam () const { return parameter.size(); }
    Parameter* get_parameter (unsigned iparam) { return parameter.at(iparam); }
    
    protected:

    //! Type of the calibrator
    Reference::To<const Calibrator::Type> type;
    
    //! The reference epoch
    MJD reference_epoch;
    //! The minimum epoch
    MJD minimum_epoch;
    //! The maximum epoch
    MJD maximum_epoch;

    //! The reference frequency in MHz
    double reference_frequency;
    //! The minimum frequency in MHz
    double minimum_frequency;
    //! The maximum frequency in MHz
    double maximum_frequency;

    std::vector< Reference::To<Parameter> > parameter;
  };
 

}

#endif
