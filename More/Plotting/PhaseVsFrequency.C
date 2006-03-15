#include "Pulsar/PhaseVsFrequencyTI.h"
#include "Pulsar/FrequencyScale.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

Pulsar::PhaseVsFrequency::PhaseVsFrequency ()
{
  get_frame()->set_y_scale( new FrequencyScale );

  isubint = 0;
  ipol = 0;
}

TextInterface::Class* Pulsar::PhaseVsFrequency::get_interface ()
{
  return new PhaseVsFrequencyTI (this);
}

void Pulsar::PhaseVsFrequency::prepare (const Archive* data)
{
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
