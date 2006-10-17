#include "Pulsar/Check.h"

// Checks registered for uses in Archive::load
Registry::List<Pulsar::Archive::Check> Pulsar::Archive::Check::registry;

#include "Pulsar/CalSource.h"
Registry::List<Pulsar::Archive::Check>::Enter<Pulsar::CalSource> calsource;
