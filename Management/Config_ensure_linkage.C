/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Config.h"
#include "Pulsar/Interpreter.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

void ensure_FrequencyAppend_linkage ();
#if HAVE_PGPLOT
void ensure_ColourMap_linkage ();
#endif

void Pulsar::Config::ensure_linkage ()
{
  // the standard shell uses a lot of different options
  Reference::To<Interpreter> temp = standard_shell();

  ensure_FrequencyAppend_linkage ();

#if HAVE_PGPLOT
  ensure_ColourMap_linkage ();
#endif
}

