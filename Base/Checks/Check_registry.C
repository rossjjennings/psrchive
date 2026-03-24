#include "Pulsar/Check.h"

void Pulsar::Archive::Check::ensure_linkage ()
{
}

#include "Pulsar/CalSource.h"
Registry::List<Pulsar::Archive::Check>::Enter<Pulsar::CalSource> calsource;

#include "Pulsar/DispersionCheck.h"
Registry::List<Pulsar::Archive::Check>::Enter<Pulsar::DispersionCheck> dedispersed;

#include "Pulsar/BirefringenceCheck.h"
Registry::List<Pulsar::Archive::Check>::Enter<Pulsar::BirefringenceCheck> deFaradayed;

