
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#if defined(__FreeBSD__) || defined(__MACH__)
#include <sys/wait.h>
#else
#include <wait.h>
#endif


#include "tempo++.h"
#include "Error.h"
#include "genutil.h"
#include "string_utils.h"
#include "fsleep.h"

// //////////////////////////////////////////////////////////////////////
// members of the Tempo namespace
//
bool   Tempo::verbose = false;
bool   Tempo::debug = false;
string Tempo::extension (".tpo");
MJD    Tempo::unspecified;

// base directory in which TEMPO will work
string Tempo::tmpdir = "/tmp/tempo/";

// file to which tempo ephemeris will be written
string Tempo::ephem_filename = "pulsar.eph";

// file to which tempo stderr will be redirected
string Tempo::stderr_filename = ".stderr";

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
    if (!userid)
      userid = unknown;

    directory = string ("/tmp/tempo/") + userid;
  }

  if (makedir (directory.c_str()) < 0)  {

    if (verbose)
      cerr << "Tempo::get_directory failure creating '" << directory 
	   << "'" << endl;

    char* home = getenv ("HOME");

    if (home)
      directory = home;
    else
      directory = ".";

    directory += "/tempo.tmp";

    if (makedir (directory.c_str()) < 0)
      throw Error (InvalidState, "Tempo::get_directory",
		   "cannot create a temporary working directory");

  }

  return directory;
}

static int lock_fd = -1;
static bool have_lock = false;

static void open_lockfile ()
{
  if (lock_fd < 0)
    lock_fd = open (Tempo::get_lockfile().c_str(), O_RDWR | O_CREAT);

  if (lock_fd < 0)
    throw Error (FailedSys, "Tempo::open_lockfile", "failed open(%s)",
		 Tempo::get_lockfile().c_str());
}

// run tempo with the given arguments
void Tempo::lock ()
{
  if (have_lock)
    return;

  open_lockfile ();

  if (flock (lock_fd, LOCK_EX) < 0)
    throw Error (FailedSys, "Tempo::lock", "failed flock(%s)",
		 get_lockfile().c_str());

  have_lock = true;
}

void Tempo::unlock ()
{
  if (!have_lock)
    return;

  open_lockfile ();

  if (flock (lock_fd, LOCK_UN) < 0)
    throw Error (FailedSys, "Tempo::unlock", "failed flock(%s)",
		 get_lockfile().c_str());

  have_lock = false;
}

string Tempo::get_lockfile () 
{
  return get_directory() + "/.lock";
}


// run tempo with the given arguments
void Tempo::tempo (const string& arguments, const string& input)
{
  char cwd[FILENAME_MAX];

  if (getcwd (cwd, FILENAME_MAX) == NULL)
    throw Error (FailedSys, "Tempo::tempo", "failed getcwd");
  
  string runtempo = get_system() + " " + arguments;

  if (!Tempo::verbose)
    runtempo += " > /dev/null 2> " + stderr_filename;

  if (!input.empty()) {
    string tmp_input = input;
    if (input[input.length()-1] != '\n')
      tmp_input += '\n';
    runtempo += " << EOD\n" + tmp_input + "EOD\n";
  }

  if (verbose)
    cerr << "Tempo::tempo system (" << runtempo << ")" << endl;

  int retries = 3;
  string errstr;

  while (retries) {    

    if (chdir (get_directory().c_str()) != 0)
      throw Error (FailedSys, "Tempo::tempo",
		   "failed chdir(" + get_directory() + ")");

    lock ();

    int err = system (runtempo.c_str());

    unlock ();

    if (chdir (cwd) != 0)
      throw Error (FailedSys, "Tempo::tempo", "failed chdir(%s)", cwd);

    if (!err)
      return;

    // else an error occured
    if (err < 0)
      errstr = strerror (err);
    else
      errstr = stringprintf ("\n\tTempo returns err code %i", WIFEXITED(err));
    
    fsleep (5e-4);
    retries --; 
  }

  // the above loop finished without a successful return
  throw Error (FailedCall, "Tempo::tempo", "system (\"" + runtempo + "\")"
	       " failed: " + errstr);
}
