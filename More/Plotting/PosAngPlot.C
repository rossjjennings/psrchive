#include "Pulsar/PosAngPlotter.h"
#include "Pulsar/PosAngPlotterTI.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"

#include <cpgplot.h>

Pulsar::PosAngPlotter::PosAngPlotter()
{
  error_bars = true;
  threshold = 3.0;

  // keep pgplot from drawing the 90
  deg_max = 89.999;
  deg_min = -89.999;
  border = 0;

  text_interface = new PosAngPlotterTI (this);
}

void Pulsar::PosAngPlotter::minmax (const Archive*, float& min, float& max)
{
  min = deg_min;
  max = deg_max;
}

void Pulsar::PosAngPlotter::draw (const Archive *data)
{
  const Integration* subint = data->get_Integration(isubint);
  Reference::To<const PolnProfile> profile = subint->new_PolnProfile(ichan);

  vector< Estimate<double> > posang;
  profile->get_PA (posang, threshold);

  float offset = -180.0;

  for (unsigned ioff=0; ioff < 3; ioff++) {

    for (unsigned ibin=0; ibin < phases.size(); ibin++)
      if (posang[ibin].get_variance() != 0)
	cpgerr1 (6, phases[ibin], posang[ibin].get_value(),
		 posang[ibin].get_error(), 1.0);
  
    offset += 180.0;

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
std::string Pulsar::PosAngPlotter::get_flux_label (const Archive* data)
{
  return "P.A. (deg.)";
}
