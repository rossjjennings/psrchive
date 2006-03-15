#include "Pulsar/PowerSpectraTI.h"
#include "Pulsar/PlotFrame.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Physical.h"

#include <cpgplot.h>
#include <values.h>

Pulsar::PowerSpectra::PowerSpectra ()
{
  isubint = ipol = 0;
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

  float min = MAXFLOAT;
  float max = 0;

  for (unsigned iprof=0; iprof < spectra.size(); iprof++) 
    for (unsigned ichan=0; ichan < spectra[iprof].size(); ichan++)
      if (spectra[iprof][ichan] != 0) {
	min = std::min( min, spectra[iprof][ichan] );
	max = std::max( max, spectra[iprof][ichan] );
      }

  if (verbose)
    cerr << "Pulsar::PowerSpectra::prepare"
      " min=" << min << " max=" << max << endl;

  get_frame()->get_y_scale()->set_minmax (min, max);
}


//! Derived classes must draw in the current viewport
void Pulsar::PowerSpectra::draw (const Archive* data)
{
  cerr << "draw" << endl;

  get_scale()->get_ordinates (data, frequencies);

  cerr << "loop" << endl;

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

