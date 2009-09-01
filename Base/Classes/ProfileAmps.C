/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileAmps.h"
#include "Pulsar/Config.h"

#include "VirtualMemory.h"
#include "malloc16.h"

using namespace std;

static Pulsar::Option<string> profile_swap_filename
(
 "Profile::swap", "",

 "Virtual memory swap space filename base",

 "The Profile class can use an alternative virtual memory manager so that \n"
 "the kernel swap space is not consumed when processing large files. \n"
 "\n"
 "This configuration parameter should be set the full path of a filename \n"
 "to be used for page swapping; e.g. \"/tmp/psrchive.swap\" \n"
 "\n"
 "The filename will have a unique extension added so that multiple \n"
 "processes will not conflict."
);

static VirtualMemory* profile_swap_init ()
{
  string filename = profile_swap_filename;

  if (filename == "")
    return 0;

  VirtualMemory* swap = new VirtualMemory (filename);

#ifdef _DEBUG
  cerr << "psrchive: virtual memory swap file=" << filename << endl;
#endif

  return swap;
}

static Reference::To<VirtualMemory> profile_swap = profile_swap_init();

/*! 
  Do not allocate memory for the amps
*/
bool Pulsar::ProfileAmps::no_amps = false;

Pulsar::ProfileAmps::ProfileAmps (unsigned _nbin)
{
#ifdef _DEBUG
  cerr << "Pulsar::ProfileAmps ctor nbin=" << nbin << endl;
#endif

  nbin = 0;
  amps = NULL;
  amps_size = 0;
  if (_nbin)
    resize( _nbin );
}

Pulsar::ProfileAmps::ProfileAmps (const ProfileAmps& copy)
{
#ifdef _DEBUG
  cerr << "Pulsar::ProfileAmps copy ctor nbin=" << copy.nbin << endl;
#endif

  nbin = 0;
  amps = NULL;
  amps_size = 0;
  if (copy.nbin) 
  {
    resize( copy.nbin );
    set_amps( copy.amps );
  }
}

static void amps_free (float* amps)
{
  if (profile_swap)
    profile_swap->destroy (amps);
  else
    free16 (amps);
}

Pulsar::ProfileAmps::~ProfileAmps () 
{
#ifdef _DEBUG
  cerr << "Pulsar::ProfileAmps dtor amps=" << amps << endl;
#endif

  if (amps != NULL) amps_free (amps); amps = 0;
}

/*
  If the size of the amps array, amps_size >= _nbin, then no new
  memory is allocated.

  If _nbin == 0, the allocated space is deleted.
*/
void Pulsar::ProfileAmps::resize (unsigned _nbin)
{
  nbin = _nbin;

  if (amps_size >= nbin && nbin != 0)
    return;

  if (amps) amps_free(amps); amps = NULL;
  amps_size = 0;

  if (nbin == 0)
    return;

  if (!no_amps)
  {
#ifdef _DEBUG
    cerr << "Pulsar::ProfileAmps::resize nbin=" << nbin << endl;
#endif

    if (profile_swap)
      amps = profile_swap->create<float> (nbin);
    else
      amps = (float*) malloc16 (sizeof(float) * nbin);

    if (!amps)
      throw Error (BadAllocation, "Pulsar::ProfileAmps::resize",
	               "failed to allocate %u floats (using %s swap space)",
				   nbin, (profile_swap) ? "custom" : "system");

    amps_size = nbin;
  }
}

//! Return a pointer to the amplitudes array
const float* Pulsar::ProfileAmps::get_amps () const
{
  if (!amps)
    throw Error (InvalidState, "Pulsar::ProfileAmps::get_amps",
		 "amplitude array not allocated");

  return amps;
}

//! Return a pointer to the amplitudes array
float* Pulsar::ProfileAmps::get_amps ()
{
  if (!amps)
    throw Error (InvalidState, "Pulsar::ProfileAmps::get_amps",
		 "amplitude array not allocated");

  return amps;
}

