/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "psrephem.h"
#include "polyco.h"
#include "tempo++.h"
#include "Error.h"

void Pulsar::Archive::set_verbosity (unsigned level)
{
  // level 3
  Tempo::verbose    = (level >= 3);
  psrephem::verbose = (level >= 3);
  polyco::verbose   = (level >= 3);
  Profile::verbose  = (level >= 3);

  Integration::verbose = (level >= 3);

  // all levels
  Archive::verbose = level;
}
