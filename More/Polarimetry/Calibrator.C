#include "Calibrator.h"

bool Pulsar::Calibrator::verbose = false;

Pulsar::Calibrator::~Calibrator ()
{
  // destructors must be defined in .C file so that the Reference::To
  // desctructor can delete forward declared objects
}
