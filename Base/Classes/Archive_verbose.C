#include "Archive.h"
#include "Integration.h"
#include "Profile.h"
#include "psrephem.h"
#include "polyco.h"
#include "tempo++.h"

bool Pulsar::Archive::verbose = false;

void Pulsar::Archive::set_verbosity (unsigned level)
{
  // level 3
  Tempo::verbose    = (level >= 3);
  psrephem::verbose = (level >= 3);
  polyco::verbose   = (level >= 3);
  Profile::verbose  = (level >= 3);

  // level 2
  Integration::verbose = (level >= 2);

  // level 1
  Archive::verbose = (level >= 1);
}
