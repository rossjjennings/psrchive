#include "Calibrator.h"

bool Pulsar::Calibrator::verbose = false;

/* The size of the window used during median filtering is given by the number
   of frequency channels, nchan, divided by median_smoothing_factor */
float Pulsar::Calibrator::median_smoothing_bandwidth = 0.25;

Pulsar::Calibrator::~Calibrator ()
{
  // destructors must be defined in .C file so that the Reference::To
  // desctructor can delete forward declared objects
}
