#include "Archive.h"
#include "Plugin.h"

Registry::List<Pulsar::Archive::Agent> Pulsar::Archive::Agent::registry;

bool Pulsar::Archive::Agent::loaded = false;

//! Null constructor
Pulsar::Archive::Agent::Agent (const char* _name)
{
  name = _name;
}

//! Destructor
Pulsar::Archive::Agent::~Agent ()
{

}

// load the plugins
static Registry::Plugin plugins;

void Pulsar::Archive::Agent::plugin_load ()
{
  plugins.load (plugin_path ());

  loaded = true;
}

/*! constructs the plugin directory name from environment variables */
string Pulsar::Archive::Agent::plugin_path ()
{
  char* env = getenv ("PSRCHIVE_PLUGINS");

  if (env)
    return env;

  env = getenv ("CVSHOME");

  if (!env)
    return "./Pulsar";

  string path = env;
  path += "/lib";

  env = getenv ("LOGIN_ARCH");
  if (env) {
    path += "/";
    path += env;
  }

  path += "/Pulsar";

  return path;
}

// reports on the status of the plugins
void Pulsar::Archive::Agent::report ()
{
  if (!loaded)
    plugin_load ();

  cerr << endl;

  if (registry.size() == 0)
    cerr << "Archive::Agent::report No Agents registered." << endl;
  else
    cerr << "Archive::Agent::report Registered Agents:" << endl;

  for (unsigned agent=0; agent<registry.size(); agent++)
    cerr << " " << registry[agent]->name << endl;

  cerr << endl;

  if (plugins.ok.size() == 0)
    cerr << "Archive::Agent::report No successfully loaded plugins." << endl;
  else
    cerr << "Archive::Agent::report Successfully loaded plugins:" << endl;

  unsigned ip = 0;

  for (ip=0; ip < plugins.ok.size(); ip++)
    cerr << "  " << plugins.ok[ip] << endl;

  if (plugins.fail.size() == 0)
    return;

  cerr << "\nArchive::Agent::report Failed plugins:" << endl;

  for (ip=0; ip < plugins.fail.size(); ip++)
    cerr << "##############################################################\n"
	 << plugins.fail[ip] << endl;
}

