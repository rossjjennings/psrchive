/***************************************************************************
 *
 *   Copyright (C) 2006-2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StokesCylindrical.h"
#include "Pulsar/Profile.h"

#include <fstream>
using namespace std;

Pulsar::StokesCylindrical::StokesCylindrical ()
{
  manage ("flux", &flux);
  flux.get_frame()->set_viewport (0,1, 0,.7);

  flux.set_plot_values  ("ILV");
  flux.set_plot_colours ("124");
  flux.set_plot_lines   ("111");

  // remove the above frame labels
  flux.get_frame()->get_label_above()->set_all ("");

  manage ("pa", &orientation);
  orientation.get_frame()->set_viewport (0,1, .7,1);

  // shorten the y label
  orientation.get_frame()->get_y_axis()->set_label("P.A. (deg.)");
  // remove the x label
  orientation.get_frame()->get_x_axis()->set_label(" ");
  // remove the x enumeration
  orientation.get_frame()->get_x_axis()->rem_opt('N');
  // remove the below frame labels
  orientation.get_frame()->get_label_below()->set_all ("");

}

//! Get the text interface to the configuration attributes
TextInterface::Parser* Pulsar::StokesCylindrical::get_interface ()
{
  return new Interface (this);
}

void Pulsar::StokesCylindrical::prepare (const Archive* data)
{
  prepare( &flux, data );
  prepare( &orientation, data );

  if (csv_filename.empty())
    return;

  const std::vector< Reference::To<const Profile> >& profs
    = flux.get_plotter()->profiles;

  const std::vector< Estimate<double> >& angles
    = orientation.AnglePlot::get_angles ();

  ofstream output (csv_filename.c_str());
  unsigned nbin = profs[0]->get_nbin();

  for (unsigned ibin=0; ibin<nbin; ibin++)
  {
    output << ibin << ",";
    for (unsigned i=0; i<profs.size(); i++)
      output << profs[i]->get_amps()[ibin] << ",";
    
    output << angles[ibin].get_value() << ","
	   << angles[ibin].get_error() << endl;
  }
}


