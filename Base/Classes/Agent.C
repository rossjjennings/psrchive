#include "Archive.h"

Registry::List<Pulsar::Archive::Agent> Pulsar::Archive::Agent::registry;

bool Pulsar::Archive::Agent::loaded = false;

//! Destructor
Pulsar::Archive::Agent::~Agent ()
{

}

