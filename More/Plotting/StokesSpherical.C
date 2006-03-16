#include "Pulsar/StokesSpherical.h"
#include "Pulsar/Profile.h"

Pulsar::StokesSpherical::StokesSpherical ()
{
  manage ("flux", &flux);
  set_viewport ( "flux", 0,1, 0,.5);

  flux.set_plot_values  ("Ip");
  flux.set_plot_colours ("12");
  flux.set_plot_lines   ("11");

  // remove the above frame labels
  flux.get_frame()->get_label_above()->set_all ("");

  manage ("pa", &orientation);
  set_viewport ( "pa", 0,1, .5,.75);

  // shorten the y label
  orientation.get_frame()->get_y_axis()->set_label("\\gh (deg.)");
  // remove the x label
  orientation.get_frame()->get_x_axis()->set_label("");
  // remove the x enumeration
  orientation.get_frame()->get_x_axis()->rem_pgbox_opt('N');
  // remove the below frame labels
  orientation.get_frame()->get_label_below()->set_all ("");
  orientation.get_frame()->get_label_above()->set_all ("");

  manage ("ell", &ellipticity);
  set_viewport ( "ell", 0,1, .75,1);

  // shorten the y label
  ellipticity.get_frame()->get_y_axis()->set_label("\\ge (deg.)");
  // remove the x label
  ellipticity.get_frame()->get_x_axis()->set_label("");
  // remove the x enumeration
  ellipticity.get_frame()->get_x_axis()->rem_pgbox_opt('N');
  // remove the below frame labels
  ellipticity.get_frame()->get_label_below()->set_all ("");

}

    //! Get the text interface to the configuration attributes
TextInterface::Class* Pulsar::StokesSpherical::get_interface ()
{
  return new Interface (this);
}

void Pulsar::StokesSpherical::prepare (const Archive*)
{
  prepare( &flux );
  prepare( &orientation );
  prepare( &ellipticity );
}


