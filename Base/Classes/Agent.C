#include "Archive.h"

Registry::List<Pulsar::Archive::Agent> Pulsar::Archive::Agent::registry;

bool Pulsar::Archive::Agent::loaded = false;

//! Null constructor
Pulsar::Archive::Agent::Agent (const char* _name)
{
  name = _name;
}

//! Destructor
Pulsar::Archive::Agent::~Agent ()
{

}

