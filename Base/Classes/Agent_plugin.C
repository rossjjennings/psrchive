#include "Archive.h"
#include "Plugin.h"

// load the plugins
static Registry::Plugin plugins;

void Pulsar::Archive::Agent::plugin_load ()
{
  plugins.load (plugin_path ());

  if (plugins.ok.size() == 0)
    plugins.load (plugin_path ("PSRHOME"));

  loaded = true;
}

/*! constructs the plugin directory name from environment variables */
string Pulsar::Archive::Agent::plugin_path (const char* environment_variable)
{
  char* env = getenv ("PSRCHIVE_PLUGINS");

  if (env)
    return env;

  env = getenv (environment_variable);

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
    cerr << " " << registry[agent]->get_name() << "\t" 
         << registry[agent]->get_description() << endl;

  cerr << endl;

  if (plugins.ok.size() == 0)
    cerr << "Archive::Agent::report No successfully loaded plugins." << endl;
  else
    cerr << "Archive::Agent::report Successfully loaded plugins:" << endl;

  unsigned ip = 0;

  for (ip=0; ip < plugins.ok.size(); ip++)
    cerr << " " << plugins.ok[ip] << endl;

  cerr << endl;

  if (plugins.fail.size() == 0)
    return;

  cerr << "Archive::Agent::report Failed plugins:" << endl;

  for (ip=0; ip < plugins.fail.size(); ip++)
    cerr << "##############################################################\n"
	 << plugins.fail[ip] << endl;

  cerr << endl;
}

