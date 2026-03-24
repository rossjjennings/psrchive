/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/NvariateScalarFactory.h"

#include "TextInterfaceParser.h"
#include "interface_factory.h"

#include <assert.h>

using namespace MEAL;
using namespace std;

static std::vector< MEAL::Nvariate<MEAL::Scalar>* >* instances = NULL;

static void instances_build ();

//! Construct a new model instance from a string
MEAL::Nvariate<MEAL::Scalar>* 
MEAL::NvariateScalarFactory::operator () (const std::string& text)
{
  if (instances == NULL)
    instances_build ();

  assert (instances != NULL);

  return TextInterface::factory< MEAL::Nvariate<MEAL::Scalar> > (*instances, text);
}

#include "MEAL/Gaussian.h"
#include "MEAL/Polynomial.h"
#include "MEAL/Polynomial2D.h"
#include "MEAL/PowerLaw.h"
#include "MEAL/ScalarArgument.h"
#include "MEAL/ScaledVonMises.h"
#include "MEAL/ScaledVonMisesDeriv.h"
#include "MEAL/Steps.h"

static void instances_build ()
{
  instances = new std::vector< MEAL::Nvariate<MEAL::Scalar>* >;

  instances->push_back( new Gaussian );
  instances->push_back( new Polynomial );
  instances->push_back( new Polynomial2D );
  instances->push_back( new PowerLaw );
  instances->push_back( new ScalarArgument );
  instances->push_back( new ScaledVonMises );
  instances->push_back( new ScaledVonMisesDeriv );
  instances->push_back( new Steps );
}

