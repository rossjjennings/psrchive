#include "Pulsar/Config.h"
#include <stdlib.h>

using namespace std;

// global configuration
Pulsar::Config Pulsar::config;

Pulsar::Config::Config ()
{
  const char* psrchive_dir = getenv ("PSRCHIVE");
  if (!psrchive_dir)
    return;

  string psrchive = psrchive_dir + string("/psrchive.cfg");

  load (psrchive);
}
