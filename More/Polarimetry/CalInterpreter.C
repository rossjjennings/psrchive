/***************************************************************************
 *
 *   Copyright (C) 2007 - 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CalInterpreter.h"
#include "Pulsar/CalibratorType.h"
#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/PolnProfile.h"

#include "Pulsar/Database.h"

#include "Pulsar/ReferenceCalibrator.h"

#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/HybridCalibrator.h"
#include "Pulsar/PolnCalibratorExtension.h"

#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/FluxCalibratorExtension.h"

#include "Pulsar/IonosphereCalibrator.h"
#include "Pulsar/FrontendCorrection.h"
#include "Pulsar/BackendCorrection.h"

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
      "usage:\n"
      "   load <filename>                 filename of database or calibrator\n" 
      "   load hybrid <obs> <pcm> [flux]  load specific hybid calibrator" );

  add_command 
    ( &CalInterpreter::match,
      "match", "set or get calibrator match criteria",
      "usage: match [key[=value]]\n"
      "    string key           name of the criterion to set/get \n"
      "    string value         value of the criterion" );

  add_command
    ( &CalInterpreter::backend,
      "backend", "backend convention correction" );

  add_command 
    ( &CalInterpreter::cal,
      "", "calibrate the current archive using the current settings" );

  add_command 
    ( &CalInterpreter::frontend,
      "frontend", "frontend (basis and projection) correction" );

  add_command 
    ( &CalInterpreter::fluxcal,
      "flux", "perform flux calibration using the current setting" );

  add_command 
    ( &CalInterpreter::set_gain,
      "gain", "normalize profile by absolute gain (ruins flux cal)" );

  add_command
    ( &CalInterpreter::fscrunch,
      "fscrunch", "integrate PolnCalibrator Extension in frequency",
      "usage: fscrunch [chans] \n"
      "  unsigned chans    number of desired frequency channels \n"
      "                    if not specified, fscrunch all (chans=1)\n" );

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
  vector<string> arguments = setup(args);
  string dbase_error;
  string cal_error;

  if (arguments[0] == "hybrid") {
    // usage: cal load hybrid <obscal file> <pcmcal or fluxcal file>
    try {

      // load reference observation
      Reference::To<Archive> obs_arch = Archive::load(arguments[1]);
      Reference::To<ReferenceCalibrator> ref_cal;
      ref_cal = new SingleAxisCalibrator(obs_arch);

      // load model or fluxcal
      Reference::To<Archive> mod_arch = Archive::load(arguments[2]);
      if ( mod_arch->get<FluxCalibratorExtension>() ) {
        // use SingleAxis+Flux model
        flux_calibrator = new FluxCalibrator(mod_arch);
        hybrid_calibrator = new HybridCalibrator;
        hybrid_calibrator->set_reference_input(
            flux_calibrator->get_CalibratorStokes());
      }
      else {
        // use full reception model; NB const below sets reference input
        hybrid_calibrator = new HybridCalibrator(mod_arch);
      }
      hybrid_calibrator->set_reference_observation(ref_cal);

      return response (Good);

    }
    catch (Error& error) {
      return response( Fail,
          "Could not construct hybrid calibrator.\n"+error.get_message());
    }
  }
        
  string filename = arguments[0];

  try 
  {
    // try to load the file as a database
    Database* temp = new Database( filename );

    string result;

    if (!database)
    {
      database = temp;
      result = filename + " database loaded";
    }
    else
    {
      database->merge(temp);
      result = filename + " database merged";
    }

    calibrator = 0;
    return response (Good, result);
  }
  catch (Error& error)
  {
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

    add( &Pulsar::Database::Criteria::get_sequence,
         &Pulsar::Database::Criteria::set_sequence,
         "order", "Use calibrator observed before/after pulsar" );

    /* TO-DO: create set|get methods for other Criteria attributes and
       include them here */
  }
};



string Pulsar::CalInterpreter::match (const string& args) try
{
  Database::Criteria criteria = Database::get_default_criteria ();

  CriteriaInterface interface (&criteria);
  string retval = interface.process(args);

  Database::set_default_criteria (criteria);

  return response (Good, retval);
}
catch (Error& error)
{
  return response (Fail, "unrecognized type '" + args + "'");
}


string Pulsar::CalInterpreter::backend (const string& args) try
{
  Pulsar::BackendCorrection correct;
  correct( get() );

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

string Pulsar::CalInterpreter::cal (const string& arg) try
{
  Reference::To<PolnCalibrator> use_cal;

  if (!calibrator && !hybrid_calibrator && !database && !transformation)
    return response( Fail, "no database, calibrator or transformation loaded");

  if (transformation)
  {
    Jones<double> xform = transformation->evaluate();
    get()->transform( xform );
  }
  else
  {
    if (hybrid_calibrator) {
      std::cout << "here!" << std::endl;
      use_cal = hybrid_calibrator;
    }
    else if (calibrator)
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

string Pulsar::CalInterpreter::set_gain (const string& args) try
{
  Pulsar::PolnProfile::normalize_weight_by_absolute_gain = true;
  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

string Pulsar::CalInterpreter::fscrunch (const string& args) try
{
  PolnCalibratorExtension* ext = get()->get<PolnCalibratorExtension>();
  if (!ext)
    return response (Fail, "Archive has no Polarization Calibrator Extension");

  fscruncher.fscrunch (this, ext, args);
    return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}


