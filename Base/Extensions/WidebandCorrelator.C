#include "Pulsar/WidebandCorrelator.h"

//! Default constructor
Pulsar::WidebandCorrelator::WidebandCorrelator ()
  : Backend ("WidebandCorrelator")
{
  name = "unknown";
  configfile = "UNSET";
  nrcvr = 0;
  tcycle = 0.0;
}

//! Copy constructor
Pulsar::WidebandCorrelator::WidebandCorrelator (const WidebandCorrelator& ext)
  : Backend ("WidebandCorrelator")
{
  operator = (ext);
}

//! Operator =
const Pulsar::WidebandCorrelator&
Pulsar::WidebandCorrelator::operator= (const WidebandCorrelator& extension)
{
  name = extension.name;
  configfile = extension.configfile;
  nrcvr = extension.nrcvr;
  tcycle = extension.tcycle;

  return *this;
}

//! Destructor
Pulsar::WidebandCorrelator::~WidebandCorrelator ()
{
}
