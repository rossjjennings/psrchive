
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <unistd.h>

#include "tempo++.h"
#include "genutil.h"

// //////////////////////////////////////////////////////////////////////
// members of the Tempo namespace
//
bool   Tempo::verbose = false;
bool   Tempo::debug = false;
string Tempo::extension (".tpo");
MJD    Tempo::unspecified;

// //////////////////////////////////////////////////////////////////////
// static storage of the tempo system call.
// can be set with Tempo::set_system 
//
static string system_call ("tempo");

// //////////////////////////////////////////////////////////////////////
// static storage of the tempo version number.
// is set on call to Tempo::set_system 
//
static float version = -1.0;

// //////////////////////////////////////////////////////////////////////
// static storage of the tempo working directory
// can be set with Tempo::set_directory
//
static string directory;

// //////////////////////////////////////////////////////////////////////
// get_system_version
//   Static function makes a system call to tempo using the current value
// of 'system_call' to query the version number
static void get_system_version ()
{
  string call = system_call + " -v";

  FILE* fptr = popen (call.c_str(),"r");
  if (!fptr) {
    fprintf (stderr, "Tempo::get_system_version failed '%s'\n", call.c_str());
    return;
  }
  if (fscanf (fptr," Tempo v %f", &version) < 1)
    fprintf (stderr, "Tempo::get_system_version failed to parse version\n");

  pclose (fptr);
}

float Tempo::get_version ()
{
  if (version < 0)
    get_system_version ();
  return version;
}

void Tempo::set_system (const char* sys_call)
{
  system_call = sys_call;
  get_system_version ();
}

string Tempo::get_system ()
{
  return system_call;
}

void Tempo::set_directory (const char* dir)
{
  directory = dir;
}


// the shittest of shit
// SUN CC 5.0 compiler cannot distinguish these overloaded functions
// from the const char* version given above
#if !defined (sun) 
void Tempo::set_system (const string& sys_call)
{
  system_call = sys_call;
  get_system_version ();
}

void Tempo::set_directory (const string& dir)
{
  directory = dir;
}
#endif


string Tempo::get_directory ()
{
  char* unknown = "unknown";

  if (!directory.length()) {
    char* userid = getenv ("USER");
    if (userid)
      userid = unknown;

    directory = string ("/tmp/tempo/") + userid;
    if (makedir (directory.c_str()) < 0)  {
      cerr << "Tempo::get_directory failure creating '" << directory 
	   << "'" << endl;
      directory.erase();
    }
  }
  return directory;
}

