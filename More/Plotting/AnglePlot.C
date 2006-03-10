#include "Pulsar/AnglePlotter.h"
#include "Pulsar/AnglePlotterTI.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"

#include <cpgplot.h>

Pulsar::AnglePlotter::AnglePlotter()
{
  error_bars = true;
  threshold = 3.0;
  range = 0.0;

  isubint = 0;
  ichan = 0;
}

TextInterface::Class* Pulsar::AnglePlotter::get_interface ()
{
  return new AnglePlotterTI (this);
}

void Pulsar::AnglePlotter::prepare (const Archive* data)
{
  get_angles (data);

  if (angles.size() != data->get_nbin())
    throw Error (InvalidState, "Pulsar::AnglePlotter::prepare",
		 "angles vector size=%u != nbin=%u",
		 angles.size(), data->get_nbin());

  if (range) {
    // keep pgplot from drawing the 90 or 180 at the edge
    float half = 0.5 * range - 0.0001;
    set_yrange (-half, half);
    return;
  }

  unsigned i_min, i_max;
  get_range_bin (data, i_min, i_max);

  set_yrange( min_element (angles.begin()+i_min, angles.begin()+i_max)->val,
	      max_element (angles.begin()+i_min, angles.begin()+i_max)->val );
}

void Pulsar::AnglePlotter::draw (const Archive *data)
{
  float offset = 0;
  unsigned times = 1;

  if (range)
    offset = -range;

  for (unsigned ioff=0; ioff < times; ioff++) {

    for (unsigned ibin=0; ibin < phases.size(); ibin++)
      if (angles[ibin].get_variance() != 0)
	if (error_bars)
	  cpgerr1 (6, phases[ibin], angles[ibin].get_value(),
		   angles[ibin].get_error(), 1.0);
	else
	  cpgpt1 (phases[ibin], angles[ibin].get_value(), 17);
  
    offset += range;

  }

#if 0
  if (degrees) {
    float half_range = (max_phase - min_phase) * 180;
    cpgswin (-half_range, +half_range, -pa_range, pa_range + plus_half);
  }

  if (publn)
    cpgbox("bcst",0,0,"bcnst",60,6);
  else
    cpgbox("bcst",0,0,"bcnvst",30,3);
#endif
}

//! Return the label for the y-axis
std::string Pulsar::AnglePlotter::get_flux_label (const Archive* data)
{
  return "P.A. (deg.)";
}
