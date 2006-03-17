/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/TimerArchive.h"
#include "timer++.h"

string Pulsar::TimerArchive::Agent::get_description ()
{
  return "TIMER Archive version 12.3";
}

//! return true if filename refers to a timer archive
bool Pulsar::TimerArchive::Agent::advocate (const char* filename)
{
  struct timer hdr;
  
  if ( Timer::fload (filename, &hdr, TimerArchive::big_endian) < 0 )
    return false;

  /* If backend-specific extensions exist, do not advocate use of this class
     and allow the backend-specific plugin to deal with it */
  if ( Timer::backend_recognized (hdr.backend) != -1)
    return false;

  return true;
}


