#include "TimerArchive.h"
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
  else
    return true;
}


