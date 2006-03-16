#include "Pulsar/StokesCylindrical.h"
#include "Pulsar/Profile.h"

Pulsar::StokesCylindrical::StokesCylindrical ()
{
  manage ("flux", &flux);
  set_viewport ( "flux", 0,1, 0,.7);

  flux.set_plot_values  ("ILV");
  flux.set_plot_colours ("124");
  flux.set_plot_lines   ("111");

  // remove the above frame labels
  flux.get_frame()->get_label_above()->set_all ("");

  manage ("pa", &orientation);
  set_viewport ( "pa", 0,1, .7,1);

  // shorten the y label
  orientation.get_frame()->get_y_axis()->set_label("P.A. (deg.)");
  // remove the x label
  orientation.get_frame()->get_x_axis()->set_label("");
  // remove the x enumeration
  orientation.get_frame()->get_x_axis()->rem_pgbox_opt('N');
  // remove the below frame labels
  orientation.get_frame()->get_label_below()->set_all ("");

}

    //! Get the text interface to the configuration attributes
TextInterface::Class* Pulsar::StokesCylindrical::get_interface ()
{
  return new Interface (this);
}

void Pulsar::StokesCylindrical::prepare (const Archive*)
{
  prepare( &flux );
  prepare( &orientation );
}


