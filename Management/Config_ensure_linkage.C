/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Config.h"

#include "Pulsar/Interpreter.h"
#include "Pulsar/FrequencyAppend.h"

void Pulsar::Config::ensure_linkage ()
{
  // the standard shell uses a lot of different options
  Reference::To<Interpreter> temp = standard_shell();

  FrequencyAppend append;
}

