#include "Pulsar/PowerSpectraTI.h"
#include "Pulsar/PlotFrame.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Physical.h"

#include <cpgplot.h>

Pulsar::PowerSpectra::PowerSpectra ()
{
  isubint = ichan = ipol = 0;
  plot_lines = true;

  get_frame()->get_y_scale()->set_buf_norm(0.05);
}
 
TextInterface::Class* Pulsar::PowerSpectra::get_interface ()
{
  return new PowerSpectraTI (this);
}

//! Derived classes must compute the minimum and maximum values (y-axis)
void Pulsar::PowerSpectra::prepare (const Archive* data)
{
  spectra.clear();

  get_spectra (data);

  if (!spectra.size())
    throw Error (InvalidState, "Pulsar::PowerSpectra::prepare",
		 "Spectra array empty after call to get_spectra");

  unsigned i_min, i_max;
  get_scale()->get_range (data, i_min, i_max);

  float min = *min_element (spectra[0].begin()+i_min, 
			    spectra[0].begin()+i_max);
  float max = *max_element (spectra[0].begin()+i_min, 
			    spectra[0].begin()+i_max);

  for (unsigned iprof=1; iprof < spectra.size(); iprof++) {

    min = std::min( min, *min_element (spectra[iprof].begin()+i_min, 
				       spectra[iprof].begin()+i_max) );
    max = std::max( max, *max_element (spectra[iprof].begin()+i_min, 
				       spectra[iprof].begin()+i_max) );

  }

  get_frame()->get_y_scale()->set_minmax (min, max);
}


//! Derived classes must draw in the current viewport
void Pulsar::PowerSpectra::draw (const Archive* data)
{
  get_scale()->get_ordinates (data, frequencies);

  for (unsigned iprof=0; iprof < spectra.size(); iprof++) {

    if (plot_sci.size() == spectra.size())
      cpgsci (plot_sci[iprof]);
    else
      cpgsci (iprof+1);

    if (plot_sls.size() == spectra.size())
      cpgsls (plot_sls[iprof]);
    else
      cpgsls (iprof+1);

    draw (spectra[iprof]);
  }

}

//! draw the profile in the current viewport and window
void Pulsar::PowerSpectra::draw (const vector<float>& data) const
{
  if (plot_lines)
    cpgline (data.size(), &frequencies[0], &data[0]);
  else
    cpgpt (data.size(), &frequencies[0], &data[0], -1);
}



//! Return the label for the y-axis
std::string Pulsar::PowerSpectra::get_ylabel (const Archive* data)
{
  if (data->get_scale() == Signal::Jansky)
    return "Flux Density (mJy)";
  else
    return "Relative Flux Units";
}

