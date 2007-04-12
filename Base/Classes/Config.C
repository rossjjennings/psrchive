/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/Config.h"

#include <stdlib.h>

using namespace std;

// global configuration
Pulsar::Config Pulsar::config;

Pulsar::Config::Config ()
{
  load (get_home() + "/psrchive.cfg");
}

string Pulsar::Config::get_runtime ()
{
  return get_home() + "/share";
}

string Pulsar::Config::get_home ()
{
  char* psrchive = getenv ("PSRCHIVE");
  if (psrchive)
    return psrchive;

  return PACKAGE_INSTALL;
}

