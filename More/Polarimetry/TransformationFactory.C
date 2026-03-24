/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/TransformationFactory.h"

#include "Pulsar/Britton2000.h"
#include "Pulsar/SingleAxis.h"
#include "MEAL/Rotation1.h"
#include "MEAL/Boost1.h"
#include "MEAL/Gain.h"

#include "TextInterfaceParser.h"
#include "interface_factory.h"

#include <assert.h>

using namespace MEAL;
using namespace std;

static std::vector< MEAL::Complex2* >* instances = NULL;

static void instances_build ()
{
  instances = new std::vector< MEAL::Complex2* >;

  instances->push_back( new MEAL::Rotation1( Vector<3,double>::basis(0) ) );
  instances->push_back( new MEAL::Boost1( Vector<3,double>::basis(0) ) );
  instances->push_back( new MEAL::Gain<Complex2>() );
  instances->push_back( new Calibration::SingleAxis );
  instances->push_back( new Calibration::Britton2000 );
}

//! Construct a new model instance from a string
MEAL::Complex2* 
Calibration::TransformationFactory::operator() (const std::string& text)
{
  if (instances == NULL)
    instances_build ();

  assert (instances != NULL);

  return TextInterface::factory<MEAL::Complex2> (*instances, text);
}

