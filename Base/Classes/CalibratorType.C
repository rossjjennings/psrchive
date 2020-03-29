/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CalibratorTypes.h"
#include "identifiable_factory.h"

Pulsar::Calibrator::Type* 
Pulsar::Calibrator::Type::factory (const std::string& name)
{
  std::vector< Reference::To<Type> > instances;
  instances.push_back( new CalibratorTypes::Flux );
  instances.push_back( new CalibratorTypes::SingleAxis );
  instances.push_back( new CalibratorTypes::van02_EqA1 );
  instances.push_back( new CalibratorTypes::van04_Eq13 );
  instances.push_back( new CalibratorTypes::van09_Eq );
  instances.push_back( new CalibratorTypes::bri00_Eq19 );
  instances.push_back( new CalibratorTypes::bri00_Eq19_iso );
  instances.push_back( new CalibratorTypes::van04_Eq18 );
  instances.push_back( new CalibratorTypes::ovhb04 );
  instances.push_back( new CalibratorTypes::ManualPoln );

  return identifiable_factory<Type> (instances, name);
}

bool Pulsar::Calibrator::Type::is_a (const Type* that) const
{
  // cerr << "typeid(this)=" << typeid(*this).name() << endl;
  // cerr << "typeid(that)=" << typeid(*that).name() << endl;

  return typeid(*this) == typeid(*that);
}
