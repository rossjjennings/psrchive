/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CalibrationInterpolatorExtension.h"
#include "Pulsar/CalibratorTypeInterface.h"
#include "templates.h"

using namespace Pulsar;

//! Default constructor
CalibrationInterpolatorExtension::CalibrationInterpolatorExtension
(const char* name) : Extension (name)
{
  reference_frequency = 0;
  minimum_frequency = 0;
  maximum_frequency = 0;
}

//! Copy constructor
CalibrationInterpolatorExtension::CalibrationInterpolatorExtension
(const CalibrationInterpolatorExtension& copy)
 : Extension (copy.get_extension_name().c_str())
{
  operator = (copy);
}

//! Operator =
const CalibrationInterpolatorExtension&
CalibrationInterpolatorExtension::operator= (const CalibrationInterpolatorExtension& copy)
{
  if (this == &copy)
    return *this;

  type = copy.type->clone();

  coupling_point = copy.coupling_point;
  
  reference_epoch = copy.reference_epoch;
  minimum_epoch = copy.minimum_epoch;
  maximum_epoch = copy.maximum_epoch;

  reference_frequency = copy.reference_frequency;
  minimum_frequency = copy.minimum_frequency;
  maximum_frequency = copy.maximum_frequency;

  parameter.resize( copy.parameter.size() );

  for (unsigned iparam = 0; iparam < parameter.size(); iparam++)
    parameter[iparam] = new Parameter( *(copy.parameter[iparam]) );
  
  return *this;
}

//! Destructor
CalibrationInterpolatorExtension::~CalibrationInterpolatorExtension ()
{
}

void CalibrationInterpolatorExtension::set_type (const Calibrator::Type* _type)
{
  type = _type;
}

const Calibrator::Type* CalibrationInterpolatorExtension::get_type () const
{
  return type;
}

CalibrationInterpolatorExtension::Parameter::Interface::Interface
(CalibrationInterpolatorExtension::Parameter *s_instance)
{
  if( s_instance )
    set_instance( s_instance );

  add( &CalibrationInterpolatorExtension::Parameter::get_code,
       "code", "Calibration model code" );

  add( &CalibrationInterpolatorExtension::Parameter::get_iparam,
       "index", "Calibration model index" );

  add( &CalibrationInterpolatorExtension::Parameter::get_log10_smoothing_factor,
       "logsm", "Base 10 logarithm of spline smoothing factor" );

  add( &CalibrationInterpolatorExtension::Parameter::get_total_chi_squared,
       "chisq", "Total chi squared" );
}


// Text interface to a CalibrationInterpolatorExtension extension
CalibrationInterpolatorExtension::Interface::Interface
  (CalibrationInterpolatorExtension *s_instance)
{
  if( s_instance )
    set_instance( s_instance );

  import( Calibrator::Type::Interface(),
	  &CalibrationInterpolatorExtension::get_type );

  add( &CalibrationInterpolatorExtension::get_reference_epoch,
       &CalibrationInterpolatorExtension::set_reference_epoch,
       "mjd", "Reference epoch of calibration interpolator" );

  add( &CalibrationInterpolatorExtension::get_reference_frequency,
       &CalibrationInterpolatorExtension::set_reference_frequency,
       "freq", "Reference frequency of calibration interpolator" );
}

//! Return a text interfaces that can be used to access this instance
TextInterface::Parser* CalibrationInterpolatorExtension::get_interface()
{
  return new CalibrationInterpolatorExtension::Interface( this );
}
