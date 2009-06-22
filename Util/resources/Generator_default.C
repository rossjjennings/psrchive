/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Predict.h"

using namespace std;

Pulsar::Predictor::Policy Pulsar::Predictor::policy = Pulsar::Predictor::Input;

Pulsar::Generator* Pulsar::Generator::default_generator = 0;

Pulsar::Generator* Pulsar::Generator::get_default ()
{
  if (!default_generator)
    default_generator = new Tempo::Predict;

  return default_generator->clone ();
}

