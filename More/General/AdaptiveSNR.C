#include "Pulsar/AdaptiveSNR.h"
#include "Pulsar/Profile.h"

Pulsar::AdaptiveSNR::AdaptiveSNR ()
{
  initial_baseline_window = Profile::default_duty_cycle;
  baseline_threshold = 5.0;
  max_iterations = 10;
}

//! Set the width of the window used to find the initial baseline
void Pulsar::AdaptiveSNR::set_initial_baseline_window (float width)
{
  if (width < 0 || width > 1.0)
    throw Error (InvalidParam,
		 "Pulsar::AdaptiveSNR::set_initial_baseline_window",
		 "invalid width = %f periods", width);

  initial_baseline_window = width;
}

//! Set the threshold below which samples are included in the baseline
void Pulsar::AdaptiveSNR::set_baseline_threshold (float sigma)
{
  if (sigma < 0)
    throw Error (InvalidParam,
		 "Pulsar::AdaptiveSNR::set_baseline_threshold",
		 "invalid threshold = %f sigma", sigma);

  baseline_threshold = sigma;
}


//! Set the (maximum?) number of iterations
void Pulsar::AdaptiveSNR::set_max_iterations (unsigned iterations)
{
  if (iterations == 0)
    throw Error (InvalidParam,
		 "Pulsar::AdaptiveSNR::set_max_iterations",
		 "invalid iterations = %d", iterations);

  max_iterations = iterations;
}


//! Return the signal to noise ratio
float Pulsar::AdaptiveSNR::get_snr (const Profile* profile)
{


}    

