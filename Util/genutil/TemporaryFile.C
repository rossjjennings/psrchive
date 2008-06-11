/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "TemporaryFile.h"
#include "Error.h"

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <algorithm>

using namespace std;

#ifdef HAVE_PTHREAD
#include <pthread.h>
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

//! The current temporary file instances
static std::set<TemporaryFile*> instances;

//! Set true when the signal handler has been installed
bool TemporaryFile::signal_handler_installed = false;

//! Abort after processing any signals
bool TemporaryFile::abort = true;

void TemporaryFile::install_signal_handler ()
{
  struct sigaction sa;

  sa.sa_handler = TemporaryFile::signal_handler;

  // allow interrupted system calls to continue
  sa.sa_flags = SA_RESTART;

  // block all signals during execution of the signal handler
  sigfillset (&(sa.sa_mask));

  // "man 7 signal" to see which signals are being handled
  for (unsigned i=1; i<16; i++)
  {
    // cannot catch SIGKILL
    if (i != SIGKILL)
      sigaction (i, &sa, NULL);
  }

  signal_handler_installed = true;
}

void remove_instance (TemporaryFile* file)
{
  file->remove ();
}

//! The signal handler ensures that all temporary files are removed
void TemporaryFile::signal_handler (int sig)
{
  cerr << "TemporaryFile::signal_handler received " << strsignal(sig) << endl;

#ifdef HAVE_PTHREAD
  pthread_mutex_lock (&mutex);
#endif

  for_each (instances.begin(), instances.end(), remove_instance);

#ifdef HAVE_PTHREAD
  pthread_mutex_unlock (&mutex);
#endif

  if (abort)
    exit (-1);
}

//! Construct with regular expression
TemporaryFile::TemporaryFile (const string& basename)
{
  // open the temporary file
  filename = basename + ".XXXXXXXX";

  fd = mkstemp (const_cast<char*> (filename.c_str()));
  if (fd < 0)
    throw Error (FailedSys, "TemporaryFile",
		 "failed mkstemp(" + filename + ")");

  removed = false;

#ifdef HAVE_PTHREAD
  pthread_mutex_lock (&mutex);
#endif

  if (!signal_handler_installed)
    install_signal_handler ();

  instances.insert (this);

#ifdef HAVE_PTHREAD
  pthread_mutex_unlock (&mutex);
#endif
}

//! Destructor
TemporaryFile::~TemporaryFile ()
{
  remove ();

#ifdef HAVE_PTHREAD
  pthread_mutex_lock (&mutex);
#endif

  instances.erase (this);

#ifdef HAVE_PTHREAD
  pthread_mutex_unlock (&mutex);
#endif
}

void TemporaryFile::close ()
{
  if (fd < 0)
    return;

  if (::close (fd) < 0)
    throw Error (FailedSys, "TemporaryFile",
		 "failed close (%d)", fd);

  fd = -1;
}

void TemporaryFile::remove ()
{
  close ();

  if (removed)
    return;

  if (unlink (filename.c_str()) < 0)
    throw Error (FailedSys, "TemporaryFile",
		 "failed unlink(" + filename + ")");

  removed = true;
}


