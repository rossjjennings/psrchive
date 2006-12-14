/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "TemporaryFile.h"
#include "Error.h"

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

//! Set true when the signal handler has been installed
bool TemporaryFile::signal_handler_installed = false;

//! Abort after processing any signals
bool TemporaryFile::abort = true;

void TemporaryFile::install_signal_handler ()
{
  struct sigaction sa;

  sa.sa_handler = TemporaryFile::signal_handler;
  sa.sa_flags = SA_RESTART;

  // "man 7 signal" to see which signals are being handled
  for (unsigned i=1; i<16; i++)
    sigaction (i, &sa, NULL);

  signal_handler_installed = true;
}

void remove_instance (TemporaryFile* file)
{
  file->remove ();
}

//! The signal handler ensures that all temporary files are removed
void TemporaryFile::signal_handler (int sig)
{
  cerr << strsignal(sig) << " signal received." << endl;

  for_each (instances.begin(), instances.end(), remove_instance);

  if (abort)
    ::abort ();
}

//! The current temporary file instances
std::set<TemporaryFile*> TemporaryFile::instances;


//! Construct with regular expression
TemporaryFile::TemporaryFile (const string& basename)
{
  // open the temporary file
  filename = basename + ".XXXXXXXX";

  fd = mkstemp (const_cast<char*> (filename.c_str()));
  if (fd < 0)
    throw Error (FailedSys, "TemporaryFile",
		 "failed mkstemp(" + filename + ")");

  if (!signal_handler_installed)
    install_signal_handler ();

  removed = false;

  instances.insert (this);
}

//! Destructor
TemporaryFile::~TemporaryFile ()
{
  remove ();
  instances.erase (this);
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
  if (removed)
    return;

  close ();

  if (unlink (filename.c_str()) < 0)
    throw Error (FailedSys, "TemporaryFile",
		 "failed unlink(" + filename + ")");

  removed = true;
}


