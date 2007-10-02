/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileAmps.h"

/*! 
  Do not allocate memory for the amps
*/
bool Pulsar::ProfileAmps::no_amps = false;

Pulsar::ProfileAmps::ProfileAmps (unsigned _nbin)
{
  nbin = 0;
  amps = NULL;
  amps_size = 0;
  if (_nbin)
    resize( _nbin );
}

Pulsar::ProfileAmps::ProfileAmps (const ProfileAmps& copy)
{
  nbin = 0;
  amps = NULL;
  amps_size = 0;
  if (copy.nbin) {
    resize( copy.nbin );
    set_amps( copy.amps );
  }
}

Pulsar::ProfileAmps::~ProfileAmps () 
{
  if (amps != NULL) delete [] amps;
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

  if (amps) delete [] amps; amps = NULL;
  amps_size = 0;

  if (nbin == 0)
    return;

  if (!no_amps) {
    amps = new float [nbin];
    if (!amps)
      throw Error (BadAllocation, "Pulsar::ProfileAmps::resize");
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

