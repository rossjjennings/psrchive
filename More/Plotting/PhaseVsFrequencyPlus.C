#include "Pulsar/PhaseVsFrequencyPlusTI.h"
#include "Pulsar/Profile.h"

Pulsar::PhaseVsFrequencyPlus::PhaseVsFrequencyPlus ()
{
  get_frame()->set_x_scale( new PhaseScale );
  get_frame()->set_y_scale( new FrequencyScale );

  ///////////////////////////////////////////////////////////////////////

  manage ("freq", &freq);
  set_viewport ( "freq", 0,.7, .3,.1);

  freq.get_frame()->set_x_scale (get_frame()->get_x_scale());
  freq.get_frame()->set_y_scale (get_frame()->get_y_scale());

  // remove the x enumeration
  freq.get_frame()->get_x_axis()->rem_pgbox_opt('N');
  // remove the alternative labeling
  freq.get_frame()->get_y_axis()->add_pgbox_opt ('C');
  freq.get_frame()->get_y_axis()->set_alternate (false);

  ///////////////////////////////////////////////////////////////////////

  manage ("flux", &flux);
  set_viewport ( "flux", 0,.7, 0,.3);

  flux.get_frame()->set_x_scale (get_frame()->get_x_scale());

  // remove the above frame labels
  flux.get_frame()->get_label_above()->set_all ("");

  ///////////////////////////////////////////////////////////////////////

  manage ("psd", &psd);
  set_viewport ( "psd", 1,.7, .3,1);

  // shorten the y label
  psd.get_frame()->get_y_axis()->set_label("Power");
  // remove the x label
  psd.get_frame()->get_x_axis()->set_label("");
  // remove the x enumeration
  psd.get_frame()->get_x_axis()->rem_pgbox_opt('N');
  // remove the below frame labels
  psd.get_frame()->get_label_below()->set_all ("");

}

//! Get the text interface to the configuration attributes
TextInterface::Class* Pulsar::PhaseVsFrequencyPlus::get_interface ()
{
  return new PhaseVsFrequencyPlusTI (this);
}

void Pulsar::PhaseVsFrequencyPlus::prepare (const Archive*)
{
  freq.set_subint (isubint);
  flux.set_subint (isubint);
  psd.set_subint (isubint);
}


