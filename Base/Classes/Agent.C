#include "Pulsar/BasicArchive.h"
#include "plugins.h"

Registry::List<Pulsar::Archive::Agent> Pulsar::Archive::Agent::registry;

bool Pulsar::Archive::Agent::loaded = false;

//! Destructor
Pulsar::Archive::Agent::~Agent ()
{
  if (verbose)
    cerr << "Pulsar::Archive::Agent::~Agent" << endl;
}

// reports on the status of the plugins
void Pulsar::Archive::Agent::report ()
{
  init ();

  cerr << endl;

  if (registry.size() == 0)
    cerr << "Archive::Agent::report No Agents registered." << endl;
  else
    cerr << "Archive::Agent::report Registered Agents:" << endl;

  for (unsigned agent=0; agent<registry.size(); agent++)
    cerr << " " << registry[agent]->get_name() << "\t" 
         << registry[agent]->get_description() << endl;

  cerr << endl;

#ifndef _PSRCHIVE_STATIC
  plugin_report ();
#endif

}

#ifndef _PSRCHIVE_STATIC

void Pulsar::Archive::Agent::init ()
{
  Pulsar::BasicArchive::ensure_linkage();

  if (!loaded)
    plugin_load ();
}

#endif  // not _PSRCHIVE_STATIC

