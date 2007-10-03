/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// #define _DEBUG 1

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

#ifdef _DEBUG
static int runme () { cerr << "Generator_default: init" << endl; return 0; }
static int test = runme ();
#endif

/*!
  policy == "input"   -> new predictors will have same type as input
  policy == "default" -> new predictors will be of the default type
*/
string Pulsar::Predictor::policy;

Pulsar::Option<string> 
policy_config_wrapper 
(
 &Pulsar::Predictor::policy, 
 "Predictor::policy", "input",

 "Policy for generating new predictors",

 "The type of predictor constructed when generating a new \n"
 "pulse phase prediction model (in Archive::update_model). \n"
 "There are currently two policies for choosing the type of \n"
 "the new predictor: \n"
 "\n"
 " 'input'   - same as the currently installed predictor \n"
 " 'default' - determined by Predictor::default"
);

string Pulsar::Predictor::default_type;

Pulsar::Option<string>
default_type_config_wrapper
(
 &Pulsar::Predictor::default_type,
 "Predictor::default", "polyco",

 "Default predictor type",

 "The default phase predictor model type: 'polyco' or 'tempo2'"
);


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
