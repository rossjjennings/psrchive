#ifdef HAVE_CONFIG_H
#include<config.h>
#endif

#include "Pulsar/Archive.h"

template<class Type>
Registry::List<Pulsar::Archive::Agent>::Enter<typename Type::Agent>
Pulsar::Archive::Advocate<Type>::entry;

Registry::List<Pulsar::Archive::Agent> Pulsar::Archive::Agent::registry;

#include "Pulsar/TimerArchive.h"
#include "Pulsar/BasebandArchive.h"
#include "Pulsar/EPNArchive.h"

#ifdef HAVE_CFITSIO
#include "Pulsar/FITSArchive.h"
#endif

void Pulsar::Archive::Agent::static_load () {

  Archive::Advocate<TimerArchive>::ensure_linkage();
  Archive::Advocate<BasebandArchive>::ensure_linkage();
  Archive::Advocate<EPNArchive>::ensure_linkage();

#ifdef HAVE_CFITSIO
  Archive::Advocate<Pulsar::FITSArchive>::ensure_linkage();
#endif

}

