#include "Archive.h"
#include "Plugin.h"

// load the plugins
static Registry::Plugin plugins;

void Pulsar::Archive::Agent::plugin_load ()
{
  if (verbose) {
    cerr << "Pulsar::Archive::Agent::plugin_load" << endl;
    Registry::Plugin::verbose = true;
  }

  char* env = getenv ("PSRCHIVE_PLUGINS");
  if (env)
    plugins.load (env);

  if (plugins.ok.size() == 0)
    plugins.load (plugin_path ("CVSHOME"));

  if (plugins.ok.size() == 0)
    plugins.load (plugin_path ("PSRHOME"));

  if (plugins.ok.size() == 0)
    plugins.load ("./Pulsar");

  loaded = true;
}

void Pulsar::Archive::Agent::plugin_report ()
{
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

  for (unsigned ip=0; ip < plugins.fail.size(); ip++)
    cerr << "##############################################################\n"
	 << plugins.fail[ip] << endl;

  cerr << endl;
}

/*! constructs the plugin directory name from environment variables */
string Pulsar::Archive::Agent::plugin_path (const char* environment_variable)
{
  char* env = getenv (environment_variable);

  string path;

  if (!env)
    return path;

  path = env;
  path += "/lib";

  env = getenv ("LOGIN_ARCH");
  if (env) {
    path += "/";
    path += env;
  }

  path += "/Pulsar";

  return path;
}

