#include "Pulsar/BasicArchive.h"

Registry::List<Pulsar::Archive::Agent> Pulsar::Archive::Agent::registry;

string Pulsar::Archive::Agent::plugin_path;

bool Pulsar::Archive::Agent::loaded = Pulsar::Archive::Agent::init ();

//! Destructor
Pulsar::Archive::Agent::~Agent ()
{
  if (verbose)
    cerr << "Pulsar::Archive::Agent::~Agent" << endl;
}

// reports on the status of the plugins
void Pulsar::Archive::Agent::report ()
{
  cerr << endl;

  if (verbose)
    cerr << "Archive::Agent::report registry=" << &registry << endl;

  if (registry.size() == 0)
    cerr << "Archive::Agent::report No Agents registered." << endl;
  else
    cerr << "Archive::Agent::report " << registry.size() 
	 << " Registered Agents:" << endl;

  for (unsigned agent=0; agent<registry.size(); agent++)
    cerr << " " << registry[agent]->get_name() << "\t" 
         << registry[agent]->get_description() << endl;

  cerr << endl;

#ifndef _PSRCHIVE_STATIC
  plugin_report ();
#endif

}

// reports on the status of the plugins
void Pulsar::Archive::Agent::verify_revisions ()
{
  unsigned agent = 0;

  while (agent < registry.size())

    if ( registry[agent]->get_revision() != Archive::get_revision() )  {

      cerr << "Pulsar::Archive::Agent::init " << registry[agent]->get_name() 
 	   << "::revision=" << registry[agent]->get_revision() 
 	   << " != Archive::revision=" << Archive::get_revision() << endl;

      registry.erase( agent );
      
    }
    else
      agent ++;
}

#ifndef _PSRCHIVE_STATIC

bool Pulsar::Archive::Agent::init () try {

  cerr << "Pulsar::Archive::Agent::init <dynamic>" << endl;
  
  Pulsar::BasicArchive::ensure_linkage();

  if (!loaded)
    plugin_load ();

  verify_revisions ();

  return true;

}
catch (Error& error)
{
  cerr << "Pulsar::Archive::Agent::init" << error << endl;
  return false;
}
catch (...)
{
  cerr << "Pulsar::Archive::Agent::init Unknown exception" << endl;
  return false;
}

#else

#include "static_plugins.code"

#endif  // not _PSRCHIVE_STATIC

