/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CalInterpreter.h"
#include "Pulsar/CalibratorType.h"

#include "Pulsar/Database.h"

#include "Pulsar/ReferenceCalibrator.h"

#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/PolnCalibratorExtension.h"

#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/FluxCalibratorExtension.h"

#include "Pulsar/IonosphereCalibrator.h"
#include "Pulsar/FrontendCorrection.h"

using namespace std;

Pulsar::CalInterpreter::CalInterpreter ()
{
  // default calibrator: single axis
  caltype = Calibrator::Type::factory ("single");

  add_command 
    ( &CalInterpreter::type,
      "type", "set or get the calibrator model type",
      "usage: type <name>\n"
      "    string name          name of the calibrator type" );

  add_command
    ( &CalInterpreter::load,
      "load", "load the database or calibrator",
      "usage: load <filename>\n"
      "    string filename      filename of database or calibrator" );

  add_command 
    ( &CalInterpreter::criteria,
      "criteria", "set or get calibrator match criteria",
      "usage: criteria [key[=value]]\n"
      "    string key           name of the criterion to set/get \n"
      "    string value         value of the criterion" );

  add_command 
    ( &CalInterpreter::cal,
      "", "calibrate the current archive using the current settings" );

  add_command 
    ( &CalInterpreter::frontend,
      "frontend", "frontend (basis and projection) correction" );

  add_command 
    ( &CalInterpreter::fluxcal,
      "flux", "perform flux calibration using the current setting" );

}

Pulsar::CalInterpreter::~CalInterpreter ()
{
}

string Pulsar::CalInterpreter::type (const string& args) try
{
  if (args.empty())
    return response (Good, "type is " + caltype->get_name());

  caltype = Calibrator::Type::factory( args );

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, "unrecognized type '" + args + "'");
}

string Pulsar::CalInterpreter::load (const string& args)
{
  string filename = setup<string>(args);
  string dbase_error;
  string cal_error;

  try {
    // try to load the file as a database
    database = new Database( filename );
    calibrator = 0;
    return response (Good);
  }
  catch (Error& error) {
    dbase_error = error.get_message();
  }

  try {
    // try to load the file as a single calibrator
    Reference::To<Archive> archive = Archive::load( filename );

    if ( archive->get<FluxCalibratorExtension>() )
      flux_calibrator = new FluxCalibrator (archive);
    else if ( archive->get<PolnCalibratorExtension>() )
      calibrator = new PolnCalibrator (archive);
    else
      calibrator = ReferenceCalibrator::factory (caltype, archive);

    return response (Good);
  }
  catch (Error& error) {
    cal_error = error.get_message();
  }

  try {
    transformation = MEAL::Function::load<MEAL::Complex2>( filename );
    return response (Good);
  }
  catch (Error& error) {
    return response( Fail,
		     "could not load 1) database 2) calibrator 3) tranform:\n"
		     "1) " + dbase_error + "\n"
		     "2) " + cal_error + "\n"
		     "3) " + error.get_message() );
  }

}

class CriteriaInterface 
  : public TextInterface::To<Pulsar::Database::Criteria>
{
public:
  CriteriaInterface (Pulsar::Database::Criteria* instance)
  {
    if (instance)
      set_instance (instance);
/*
 TO-DO: - add set/get methods to Criteria class
        - define Criteria::Policy insertion and extraction operators

    add( &Pulsar::Database::Criteria::get_policy,
         &Pulsar::Database::Criteria::set_policy,
         "order", "Use calibrator observed before/after pulsar" );
*/
  }
};



string Pulsar::CalInterpreter::criteria (const string& args) try
{
  Database::Criteria criteria = Database::get_default_criteria ();

  if (args.empty())
    return response (Good, "type is " + caltype->get_name());

  Database::set_default_criteria (criteria);

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, "unrecognized type '" + args + "'");
}

string Pulsar::CalInterpreter::cal (const string& arg) try
{
  Reference::To<PolnCalibrator> use_cal;

  if (!calibrator && !database && !transformation)
    return response( Fail, "no database, calibrator or transformation loaded");

  if (transformation)
  {
    Jones<double> xform = transformation->evaluate();
    get()->transform( xform );
  }
  else
  {
    if (calibrator)
      use_cal = calibrator;
    else
      use_cal = database -> generatePolnCalibrator( get(), caltype );
    
    use_cal->calibrate( get() );
  }

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

string Pulsar::CalInterpreter::fluxcal (const string& arg) try
{
  Reference::To<FluxCalibrator> use_cal;

  if (!flux_calibrator && !database)
    return response( Fail, "no database or flux calibrator loaded");

  if (flux_calibrator)
    use_cal = flux_calibrator;
  else
    use_cal = database -> generateFluxCalibrator( get() );
    
  use_cal->calibrate( get() );

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

string Pulsar::CalInterpreter::frontend (const string& args) try
{
  Pulsar::FrontendCorrection correct;
  correct.calibrate( get() );

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

