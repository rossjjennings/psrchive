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

namespace Tempo
{
  int config ()
  {
#ifdef _DEBUG
    cerr << "Tempo::config" << endl;
#endif
    return 0;
  }
}

/* ***********************************************************************

   Predictor::policy configuration

   *********************************************************************** */

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

/* ***********************************************************************

   Predictor::default_type configuration

   *********************************************************************** */

Pulsar::Option<string>
default_type_config_wrapper
(
 &Pulsar::Predictor::default_type,
 "Predictor::default", "polyco",

 "Default predictor type",

 "The default phase predictor model type: 'polyco' or 'tempo2'"
);

/* ***********************************************************************

   Tempo::Predict::minimum_nspan configuration

   *********************************************************************** */

Pulsar::Option<unsigned>
minimum_nspan_config_wrapper
(
 &Tempo::Predict::minimum_nspan,
 "Tempo::minimum_nspan", 0,

 "Minimum value of 'nspan' [minutes]",

 "When generating a polyco with 'tempo -z', the following error can occur\n"
 "\n"
 "  STOP  Nspan too small statement executed\n"
 "\n"
 "This parameter, if set, puts a lower limit on the value of 'nspan' used."
);


/* ***********************************************************************

   Tempo::Predict::maximum_rms configuration

   *********************************************************************** */

Pulsar::Option<double>
maximum_rms_config_wrapper
(
 &Tempo::Predict::maximum_rms,
 "Tempo::maximum_rms", 1e-5,    // default is 10 microturns

 "Maximum value of fit rms residual [turns]",

 "When generating a polyco with 'tempo -z', the rms between the best-fit \n"
 "polynomial and the model is reported.  If the maximum_rms is not zero, \n"
 "a warning will be issued if the rms reported by tempo exceeds this value."
);

