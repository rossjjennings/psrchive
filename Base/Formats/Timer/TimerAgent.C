#include "TimerArchive.h"
#include "timer++.h"

//! Register the TimerArchive Agent
static Registry::List<Pulsar::Archive::Agent>::Enter<Pulsar::TimerAgent> entry;

Pulsar::TimerAgent::TimerAgent ()
  : Pulsar::Archive::Agent ("TIMER Archive version 12.3") {}

//! return true if filename refers to a timer archive
bool Pulsar::TimerAgent::advocate (const char* filename)
{
  struct timer hdr;
  
  if ( Timer::fload (filename, &hdr, TimerArchive::big_endian) < 0 )
    return false;
  else
    return true;
}

//! Return a null-constructed instance of a TimerArchive
Pulsar::Archive* Pulsar::TimerAgent::new_Archive () 
{
  return new TimerArchive;
}


