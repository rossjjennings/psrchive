#include "Pulsar/Smooth.h"
#include "Pulsar/Profile.h"

#include <memory>

Pulsar::Smooth::Smooth ()
{
  duty_cycle = Pulsar::Profile::default_duty_cycle;
}

Pulsar::Smooth::~Smooth ()
{

}

void Pulsar::Smooth::set_duty_cycle (float _duty_cycle)
{
  if (_duty_cycle <= 0 || _duty_cycle >= 1.0)
    throw Error (InvalidParam, "Pulsar::Smooth::set_duty_cycle",
		 "invalid duty_cycle = %f", _duty_cycle);

  duty_cycle = _duty_cycle;
}

float Pulsar::Smooth::get_duty_cycle () const
{
  return duty_cycle;
}

void Pulsar::Smooth::transform (Profile* profile)
{
  unsigned nbin = profile->get_nbin();

  // one half of the window over which the mean will be calculated
  const unsigned halfwidth = unsigned (duty_cycle * nbin) / 2;

  // the complete width of the window
  const unsigned width = 2*halfwidth + 1;

  if (halfwidth == 0)
    throw Error (InvalidParam, "Pulsar::Smooth::transform",
		 "duty_cycle=%f and nbin=%d results in width=%d",
		 duty_cycle, width, nbin);

  if (Pulsar::Profile::verbose)
    cerr << "Pulsar::Smooth::transform duty_cycle=" << duty_cycle
	 << " width=" << width << endl;

  auto_ptr<float> temp( new float [nbin + 2*halfwidth] );
  float* amps = profile->get_amps();

  unsigned ibin;

  // copy the last halfwidth points to the front of the temporary array
  for (ibin=0; ibin < halfwidth; ibin++)
    temp.get()[ibin] = amps[nbin-halfwidth+ibin];

  // append the entire profile, starting with the first bin
  for (ibin=0; ibin < nbin; ibin++)
    temp.get()[halfwidth+ibin] = amps[ibin];

  // append the first halfwidth points
  for (ibin=0; ibin < halfwidth; ibin++)
    temp.get()[nbin+halfwidth+ibin] = amps[ibin];

  smooth_data (nbin, amps, width, temp.get());

}
 
