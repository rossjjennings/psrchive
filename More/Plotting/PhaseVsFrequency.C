#include "Pulsar/PhaseVsFrequencyTI.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

Pulsar::PhaseVsFrequency::PhaseVsFrequency ()
{
  isubint = 0;
  ipol = 0;
  text_interface = new PhaseVsFrequencyTI (this);
}

void Pulsar::PhaseVsFrequency::prepare (const Archive* data)
{
  float bw  = data -> get_bandwidth();
  float cf  = data -> get_centre_frequency();

  set_yrange (cf-0.5*bw, cf+0.5*bw);
}

std::string Pulsar::PhaseVsFrequency::get_ylabel (const Archive* data)
{
  return "Frequency (MHz)";
}

unsigned Pulsar::PhaseVsFrequency::get_nrow (const Archive* data)
{
  return data->get_nchan();
}

const Pulsar::Profile*
Pulsar::PhaseVsFrequency::get_Profile (const Archive* data, unsigned ichan)
{
  return data->get_Profile (isubint, ipol, ichan);
}
