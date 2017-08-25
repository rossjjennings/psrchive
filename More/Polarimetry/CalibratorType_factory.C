/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CalibratorTypes.h"

#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/PolarCalibrator.h"

Pulsar::Calibrator::Type* 
Pulsar::Calibrator::Type::factory (const Calibrator* calibrator)
{
  if ( calibrator->is_a<SingleAxisCalibrator>() )
    return new CalibratorTypes::SingleAxis;

  if ( calibrator->is_a<PolarCalibrator>() )
    return new CalibratorTypes::van02_EqA1;

  throw Error (InvalidState, "Pulsar::Calibrator::Type::factory",
	       "unknown calibrator type");
}

#include "Pulsar/Britton2000.h"
#include "Pulsar/SingleAxis.h"
#include "Pulsar/Instrument.h"
#include "Pulsar/ManualPolnCalibrator.h"
#include "MEAL/Polar.h"
  
MEAL::Complex2* Pulsar::new_transformation( const Calibrator::Type* type )
{
  if (type->is_a<CalibratorTypes::SingleAxis>())
    return new Calibration::SingleAxis;

  if (type->is_a<CalibratorTypes::van02_EqA1>())
    return new MEAL::Polar;

  if (type->is_a<CalibratorTypes::van09_Eq>())
    return new MEAL::Polar;

  if (type->is_a<CalibratorTypes::van04_Eq18>())
    return new Calibration::Instrument;

  if (type->is_a<CalibratorTypes::bri00_Eq19>())
    return new Calibration::Britton2000;

  if (type->is_a<CalibratorTypes::bri00_Eq19_iso>())
    return new Calibration::Britton2000 (true);
  
  //if (type->is_a<CalibratorTypes::ManualPoln>())
  //  return new Pulsar::ManualPolnCalibrator;

  throw Error (InvalidState,
               "Pulsar::new_transformation",
               "unrecognized Calibrator::Type=" + type->get_name());
}

Pulsar::Calibrator::Type*
Pulsar::new_CalibratorType( const MEAL::Complex2* xform ) 
{
  if (dynamic_cast<const Calibration::SingleAxis*>( xform ))
    return new CalibratorTypes::SingleAxis;

  if (dynamic_cast<const MEAL::Polar*>( xform ))
    return new CalibratorTypes::van02_EqA1;

  if (dynamic_cast<const Calibration::Instrument*>( xform ))
    return new CalibratorTypes::van04_Eq18;

  const Calibration::Britton2000* bri00 = 0;
  bri00 = dynamic_cast<const Calibration::Britton2000*>( xform );
  if (bri00)
  {
    if (bri00->get_degeneracy_isolated())
      return new CalibratorTypes::bri00_Eq19_iso;
    else
      return new CalibratorTypes::bri00_Eq19;
  }
  
  throw Error (InvalidState,
               "Pulsar::new_CalibratorType",
               "unrecognized transformation=" + xform->get_name());
}

#include "Factory.h"

MEAL::Complex2* Pulsar::transformation_factory (const std::string& name)
{
  return new_transformation( Pulsar::Calibrator::Type::factory (name) );
}
 
MEAL::Complex2* Pulsar::load_transformation (const std::string& filename) try
{
  Functor< MEAL::Complex2*(std::string) > constructor (&transformation_factory);
  return Factory::load (filename, constructor, Pulsar::Calibrator::verbose);
}
catch (Error& error)
{
  throw error += "MEAL::Function::load";
}

