/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Agent.h"

#ifdef HAVE_CONFIG_H
#include<config.h>
#endif

using namespace Pulsar;

Registry::List<Archive::Agent> Archive::Agent::registry;

template<class Type>
Registry::List<Archive::Agent>::Enter<typename Type::Agent>
Archive::Advocate<Type>::entry;

#include "Pulsar/TimerArchive.h"

template
Registry::List<Archive::Agent>::Enter<TimerArchive::Agent>
Archive::Advocate<TimerArchive>::entry;

#include "Pulsar/BasebandArchive.h"

template
Registry::List<Archive::Agent>::Enter<BasebandArchive::Agent>
Archive::Advocate<BasebandArchive>::entry;

#include "Pulsar/EPNArchive.h"

template
Registry::List<Archive::Agent>::Enter<EPNArchive::Agent>
Archive::Advocate<EPNArchive>::entry;

#ifdef HAVE_CFITSIO

#include "Pulsar/FITSArchive.h"

template
Registry::List<Archive::Agent>::Enter<FITSArchive::Agent>
Archive::Advocate<FITSArchive>::entry;

#endif

#include "Pulsar/ASCIIArchive.h"

template
Registry::List<Archive::Agent>::Enter<ASCIIArchive::Agent>
Archive::Advocate<ASCIIArchive>::entry;

void Archive::Agent::static_load ()
{

}

