#include "Pulsar/Calibrator.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Archive.h"

bool Pulsar::Calibrator::verbose = false;

/*! The size of the window used during median filtering is given by
   the number of frequency channels, nchan, multiplied by
   median_smoothing.  If set to zero, no smoothing is performed.  A
   sensible value is around 0.05. */
float Pulsar::Calibrator::median_smoothing = 0.0;


Pulsar::Calibrator::Calibrator ()
{
}

Pulsar::Calibrator::~Calibrator ()
{
}

//! Provide access to Integration::transform
void Pulsar::Calibrator::calibrate (Integration* integration,
				    const vector< Jones<float> >& response)
{
  integration->transform (response);
}

//! Return a const reference to the calibrator archive
const Pulsar::Archive* Pulsar::Calibrator::get_Archive () const
{
  if (!calibrator)
    throw Error (InvalidState, "Pulsar::Calibrator::get_Archive",
		 "no calibrator Archive");

  return calibrator; 
}
