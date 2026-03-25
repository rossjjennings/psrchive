/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// #define _DEBUG 1

#include "Pulsar/IntegrationManagerExpert.h"

int main ()
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
  Pulsar::IntegrationManager* manager = 0;
#pragma GCC diagnostic pop

  if (manager)
    manager->expert()->sort();

  return 0;
}
