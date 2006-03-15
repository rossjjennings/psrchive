#include "Pulsar/FrequencyScale.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

Pulsar::FrequencyScale::FrequencyScale ()
{
}

void Pulsar::FrequencyScale::get_range (const Archive* data,
					float& min, float& max) const
{
  double freq = data->get_centre_frequency();
  double bw = data->get_bandwidth();
  const_cast<FrequencyScale*>(this)->set_minmax (freq-0.5*bw, freq+0.5*bw);
  PlotScale::get_range (data, min, max);
}

void Pulsar::FrequencyScale::get_range (const Archive* data, 
					unsigned& min, unsigned& max) const
{
  PlotScale::get_range (data->get_nchan(), min, max);
}

std::string Pulsar::FrequencyScale::get_label ()
{
  return "Frequency (MHz)";
}

void Pulsar::FrequencyScale::get_ordinates (const Archive* data,
					    std::vector<float>& x_axis) const
{
  x_axis.resize (data->get_nchan());

  double freq = data->get_centre_frequency();
  double bw = data->get_bandwidth();
  double min_freq = freq - 0.5 * bw;
  for (unsigned ibin = 0; ibin < x_axis.size(); ibin++)
    x_axis[ibin] = min_freq + bw * (double(ibin) + 0.5) / x_axis.size();
}

