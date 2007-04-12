/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/Home.h"

using namespace std;

string Pulsar::Home::get_home ()
{
  char* psrchive = getenv ("PSRCHIVE");
  if (psrchive)
    return psrchive;

  return PACKAGE_INSTALL;
}

string Pulsar::Home::get_runtime ()
{
  return get_home() + "/share";
}
