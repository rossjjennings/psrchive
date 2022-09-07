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

//! Returns true if the Extension contains useful data
bool CalibrationInterpolatorExtension::has_data () const 
{ 
  return parameter.size() > 0; 
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

  add( &CalibrationInterpolatorExtension::Parameter::get_ndat_input,
       "nin", "Number of input data" );
      
  add( &CalibrationInterpolatorExtension::Parameter::get_ndat_flagged_before,
       "nflag_before", "Number of data flagged before fitting" );
  
  add( &CalibrationInterpolatorExtension::Parameter::get_ndat_flagged_after, 
       "nflag_after", "Number of data flagged after fitting" );
}


// Text interface to a CalibrationInterpolatorExtension extension
CalibrationInterpolatorExtension::Interface::Interface
  (CalibrationInterpolatorExtension *s_instance)
{
  if( s_instance )
    set_instance( s_instance );

  import( Calibrator::Type::Interface(),
	  &CalibrationInterpolatorExtension::get_type );

  add( &CalibrationInterpolatorExtension::get_coupling_point,
       &CalibrationInterpolatorExtension::set_coupling_point,
       "coupling", "Point at which reference source is coupled" );

  add( &CalibrationInterpolatorExtension::get_reference_epoch,
       &CalibrationInterpolatorExtension::set_reference_epoch,
       "mjd", "Reference epoch" );

  add( &CalibrationInterpolatorExtension::get_nreceptor,
       &CalibrationInterpolatorExtension::set_nreceptor,
       "nr", "Number of receptors" );

  add( &CalibrationInterpolatorExtension::get_native_scale,
       &CalibrationInterpolatorExtension::set_native_scale,
       "scale", "Fluxcal scale (1=Native 0=Reference)" );

  add( &CalibrationInterpolatorExtension::get_minimum_epoch,
       &CalibrationInterpolatorExtension::set_minimum_epoch,
       "minmjd", "Minimum epoch spanned" );
  
  add( &CalibrationInterpolatorExtension::get_maximum_epoch,
       &CalibrationInterpolatorExtension::set_maximum_epoch,
       "maxmjd", "Maximum epoch spanned" );

  add( &CalibrationInterpolatorExtension::get_reference_frequency,
       &CalibrationInterpolatorExtension::set_reference_frequency,
       "freq", "Reference frequency" );

  add( &CalibrationInterpolatorExtension::get_minimum_frequency,
       &CalibrationInterpolatorExtension::set_minimum_frequency,
       "minfreq", "Minimum frequency spanned" );

  add( &CalibrationInterpolatorExtension::get_maximum_frequency,
       &CalibrationInterpolatorExtension::set_maximum_frequency,
       "maxfreq", "Maximum frequency spanned" );

  typedef CalibrationInterpolatorExtension::Parameter*
  (CalibrationInterpolatorExtension::*get_mutable) (unsigned);
  
  import ( "param", new Parameter::Interface(),
	   (get_mutable) &CalibrationInterpolatorExtension::get_parameter,
	   &CalibrationInterpolatorExtension::get_nparam );
}

//! Return a text interfaces that can be used to access this instance
TextInterface::Parser* CalibrationInterpolatorExtension::get_interface()
{
  return new CalibrationInterpolatorExtension::Interface( this );
}


std::ostream& Pulsar::operator << (std::ostream& ostr,
				   CalibrationInterpolatorExtension::Parameter::Type type)
{
  switch (type)
  {
  case CalibrationInterpolatorExtension::Parameter::FrontendParameter:
  return ostr << "Frontend";
  case CalibrationInterpolatorExtension::Parameter::CalibratorStokesParameter:
  return ostr << "CalStokes";
  default:
  return ostr << "unknown";
  }
}
