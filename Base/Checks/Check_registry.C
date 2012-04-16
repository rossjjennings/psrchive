#include "Pulsar/Check.h"

#include "Pulsar/CalSource.h"
Registry::List<Pulsar::Archive::Check>::Enter<Pulsar::CalSource> calsource;

#include "Pulsar/Dedispersed.h"
Registry::List<Pulsar::Archive::Check>::Enter<Pulsar::Dedispersed> dedispersed;

#include "Pulsar/DeFaradayed.h"
Registry::List<Pulsar::Archive::Check>::Enter<Pulsar::DeFaradayed> deFaradayed;

