#include "Pulsar/Calibrator.h"
#include "Pulsar/Integration.h"

bool Pulsar::Calibrator::verbose = false;

/* The size of the window used during median filtering is given by the number
   of frequency channels, nchan, multiplied by median_smoothing */
float Pulsar::Calibrator::median_smoothing = 0.05;

Pulsar::Calibrator::~Calibrator ()
{
  // destructors must be defined in .C file so that the Reference::To
  // desctructor can delete forward declared objects
}

//! Provide access to Integration::transform
void Pulsar::Calibrator::calibrate (Integration* integration,
				    const vector< Jones<float> >& response)
{
  integration->transform (response);
}
