/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Function.h"

#include "MEAL/Polynomial.h"
#include "MEAL/Gaussian.h"
#include "MEAL/SumRule.h"

#include "MEAL/Rotation1.h"
#include "MEAL/Boost1.h"
#include "MEAL/Depolarizer.h"

#include "TextInterfaceParser.h"
#include "interface_factory.h"

#include <assert.h>

using namespace MEAL;
using namespace std;

static std::vector< MEAL::Function* >* instances = NULL;

static void instances_build ()
{
  instances = new std::vector< MEAL::Function* >;
  instances->push_back( new MEAL::Polynomial );
  instances->push_back( new MEAL::Gaussian );
  instances->push_back( new MEAL::SumRule<Scalar> );

  instances->push_back( new MEAL::Rotation1( Vector<3,double>::basis(0) ) );
  instances->push_back( new MEAL::Boost1( Vector<3,double>::basis(0) ) );
  instances->push_back( new MEAL::Depolarizer );
}

//! Construct a new model instance from a string
MEAL::Function* MEAL::Function::factory (const std::string& text)
{
  if (instances == NULL)
    instances_build ();

  assert (instances != NULL);

  return TextInterface::factory<MEAL::Function> (*instances, text);
}

