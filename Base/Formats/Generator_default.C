/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Pulsar/Predictor.h"
#include "Pulsar/Config.h"

#ifdef HAVE_TEMPO2
#include "T2Generator.h"
#endif
#include "Predict.h"

using namespace std;

/*!
  policy == "input"   -> new predictors will have same type as input
  policy == "default" -> new predictors will be of the default type
*/
string Pulsar::Predictor::policy
= Pulsar::Config::get<string>("Predictor::policy", "input");

string Pulsar::Predictor::default_type
= Pulsar::Config::get<string>("Predictor::default", "polyco");

Pulsar::Generator* Pulsar::Generator::get_default ()
{
  if (Predictor::verbose)
    cerr << "Pulsar::Generator::get_default type="
	 << Predictor::default_type << endl;

#ifdef HAVE_TEMPO2
  if (Predictor::default_type == "tempo2")
    return new Tempo2::Generator;
#endif

  return new Tempo::Predict;
}
