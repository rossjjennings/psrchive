#include "Pulsar/Archive.h"

#ifdef HAVE_CONFIG_H
#include<config.h>
#endif

Registry::List<Pulsar::Archive::Agent> Pulsar::Archive::Agent::registry;

template<class Type>
Registry::List<Pulsar::Archive::Agent>::Enter<typename Type::Agent>
Pulsar::Archive::Advocate<Type>::entry;

#include "Pulsar/TimerArchive.h"

template
Registry::List<Pulsar::Archive::Agent>::Enter<Pulsar::TimerArchive::Agent>
Pulsar::Archive::Advocate<Pulsar::TimerArchive>::entry;

#include "Pulsar/BasebandArchive.h"

template
Registry::List<Pulsar::Archive::Agent>::Enter<Pulsar::BasebandArchive::Agent>
Pulsar::Archive::Advocate<Pulsar::BasebandArchive>::entry;

#include "Pulsar/EPNArchive.h"

template
Registry::List<Pulsar::Archive::Agent>::Enter<Pulsar::EPNArchive::Agent>
Pulsar::Archive::Advocate<Pulsar::EPNArchive>::entry;

#ifdef HAVE_CFITSIO

#include "Pulsar/FITSArchive.h"

template
Registry::List<Pulsar::Archive::Agent>::Enter<Pulsar::FITSArchive::Agent>
Pulsar::Archive::Advocate<Pulsar::FITSArchive>::entry;

#endif

#include "Pulsar/ASCIIArchive.h"

template
Registry::List<Pulsar::Archive::Agent>::Enter<Pulsar::ASCIIArchive::Agent>
Pulsar::Archive::Advocate<Pulsar::ASCIIArchive>::entry;

void Pulsar::Archive::Agent::static_load ()
{

}

