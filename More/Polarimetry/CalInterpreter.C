/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CalInterpreter.h"

#include "Pulsar/Database.h"
#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/IonosphereCalibrator.h"
#include "Pulsar/CorrectionsCalibrator.h"

using namespace std;

Pulsar::CalInterpreter::CalInterpreter ()
{
  // default calibrator type
  type = Calibrator::SingleAxis;

  add_command 
    ( &CalInterpreter::cal, 'c',
      "cal", "calibrate data using various models",
      "usage:\n"
      "  cal                    calibrate the current archive \n"
      "  cal type <name>        select the type of calibrator \n"
      "    string name          name of the calibrator type \n"
      "  cal load <filename>    load the database or calibrator \n"
      "    string filename      filename of database or calibrator \n" );
}

Pulsar::CalInterpreter::~CalInterpreter ()
{
}


string Pulsar::CalInterpreter::cal (const string& args)
{
  vector<string> arguments = setup (args);

  if (arguments.size() == 2 && arguments[0] == "load")
    return load ( arguments[1] );

  if (arguments.size() == 2 && arguments[0] == "type") {
    type = fromstring<Calibrator::Type>( arguments[1].c_str() );
    return response (Good);
  }

  if (arguments.size() == 1 && arguments[0] == "type")
    return response (Good, "type is " + tostring(type));

  if (arguments.size() == 0)
    return calibrate ();
  
  return response (Fail, help("cal"));
}


string Pulsar::CalInterpreter::load (const string& arg)
{
  string dbase_error;

  try {
    // try to load the file as a database
    database = new Database( arg );
    calibrator = 0;
    return response (Good);
  }
  catch (Error& error) {
    dbase_error = error.get_message();
  }

  try {
    // try to load the file as a single calibrator
    Reference::To<Archive> archive = Archive::load( arg );
    calibrator = new PolnCalibrator (archive);
    database = 0;
    return response (Good);
  }
  catch (Error& error) {
    return response( Fail, "could not load 1) database or 2) calibrator:\n"
		     "1) " + dbase_error + "\n"
		     "2) " + error.get_message() );
  }

}

string Pulsar::CalInterpreter::calibrate () try {

  Reference::To<PolnCalibrator> use_cal;

  if (!calibrator && !database)
    return response( Fail, "no database or calibrator loaded");

  if (calibrator)
    use_cal = calibrator;
  else
    use_cal = database -> generatePolnCalibrator( get(), type );

  use_cal->calibrate( get() );

  return response (Good);

}
catch (Error& error) {
  return response (Fail, error.get_message());
}


