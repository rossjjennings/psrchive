/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ProfileVectorPlotter.h"
#include "Pulsar/PlotFrame.h"
#include "Pulsar/Profile.h"

#include <cpgplot.h>

Pulsar::ProfileVectorPlotter::ProfileVectorPlotter ()
{
  plot_histogram = false;
}

void Pulsar::ProfileVectorPlotter::minmax (PlotFrame* frame) const
{
  if (!profiles.size())
    throw Error (InvalidState, "Pulsar::ProfileVectorPlotter::minmax",
		 "no Profiles");

  unsigned nbin = profiles[0]->get_nbin();

  unsigned i_min, i_max;

  frame->get_x_scale()->get_range (nbin, i_min, i_max);

  float min = profiles[0]->min(i_min, i_max);
  float max = profiles[0]->max(i_min, i_max);

  for (unsigned iprof=1; iprof < profiles.size(); iprof++) {
    float pmin = profiles[iprof]->min(i_min, i_max);
    float pmax = profiles[iprof]->max(i_min, i_max);
    if (pmin < min)
      min = pmin;
    if (pmax > max)
      max = pmax;
  }

  frame->get_y_scale()->set_minmax (min, max);
}

void Pulsar::ProfileVectorPlotter::draw () const
{
  for (unsigned iprof=0; iprof < profiles.size(); iprof++) {

    if (plot_sci.size() == profiles.size())
      cpgsci (plot_sci[iprof]);
    else
      cpgsci (iprof+1);

    if (plot_sls.size() == profiles.size())
      cpgsls (plot_sls[iprof]);
    else
      cpgsls (iprof+1);

    draw (profiles[iprof]);
  }
}

//! draw the profile in the current viewport and window
void Pulsar::ProfileVectorPlotter::draw (const Profile* profile) const
{
  if (plot_histogram)
    cpgbin (profile->get_nbin(), &x[0], profile->get_amps(), true);
  else
    cpgline (profile->get_nbin(), &x[0], profile->get_amps());
}
