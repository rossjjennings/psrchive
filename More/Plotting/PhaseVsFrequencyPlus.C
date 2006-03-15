#include "Pulsar/PhaseVsFrequencyPlusTI.h"
#include "Pulsar/Profile.h"

Pulsar::PhaseVsFrequencyPlus::PhaseVsFrequencyPlus ()
{
  get_frame()->set_x_scale( new PhaseScale );
  get_frame()->set_y_scale( new FrequencyScale );

  ///////////////////////////////////////////////////////////////////////

  manage ("freq", &freq);
  set_viewport ( "freq", 0,.7, 0,.7);

  freq.get_frame()->set_x_scale (get_frame()->get_x_scale());
  freq.get_frame()->set_y_scale (get_frame()->get_y_scale());

  // remove the alternative labeling
  freq.get_frame()->get_y_axis()->add_pgbox_opt ('C');
  freq.get_frame()->get_y_axis()->set_alternate (false);
  // remove the above frame labels
  freq.get_frame()->get_label_above()->set_all ("");

  ///////////////////////////////////////////////////////////////////////

  manage ("flux", &flux);
  set_viewport ( "flux", 0,.7, .7,1);

  // flux plotter shares the x axis
  flux.get_frame()->set_x_scale (get_frame()->get_x_scale());

  // tell the flux plotter to integrate over all frequencies
  flux.set_chan (PlotIndex (0, true));
  
  // remove the x enumeration
  flux.get_frame()->get_x_axis()->rem_pgbox_opt('N');
  // remove the x label
  flux.get_frame()->get_x_axis()->set_label("");
  // remove the y enumeration
  flux.get_frame()->get_y_axis()->rem_pgbox_opt('N');
  // remove the y label
  flux.get_frame()->get_y_axis()->set_label("");
  // remove the above frame labels
  flux.get_frame()->get_label_above()->set_all ("");
  // put the filename in the upper left
  flux.get_frame()->get_label_above()->set_left ("=file");

  ///////////////////////////////////////////////////////////////////////

  manage ("psd", &psd);
  set_viewport ( "psd", .7,1, 0,.7);

  // spectrum plotter shares the y axis on its x axis
  psd.get_frame()->set_x_scale (get_frame()->get_y_scale());

  // tell the spectrum plotter to sum over all phase bins
  psd.set_bin (PlotIndex (0, true));

  // remove the x enumeration
  psd.get_frame()->get_x_axis()->rem_pgbox_opt('N');
  // remove the x label
  psd.get_frame()->get_x_axis()->set_label("");
  // remove the y enumeration
  //psd.get_frame()->get_y_axis()->rem_pgbox_opt('N');
  // shorten the y label
  psd.get_frame()->get_y_axis()->set_label("Power");
  // remove the below frame labels
  psd.get_frame()->get_label_below()->set_all ("");
  // remove the above frame labels
  psd.get_frame()->get_label_above()->set_all ("");

  // transpose the frame
  psd.get_frame()->set_transpose ();
}

//! Get the text interface to the configuration attributes
TextInterface::Class* Pulsar::PhaseVsFrequencyPlus::get_interface ()
{
  return new PhaseVsFrequencyPlusTI (this);
}

void Pulsar::PhaseVsFrequencyPlus::prepare (const Archive*)
{
  prepare (&freq);
  prepare (&flux);
  prepare (&psd);
}

//! Get the scale
Pulsar::PhaseScale* Pulsar::PhaseVsFrequencyPlus::get_scale ()
{
  PhaseScale* scale = 0;
  scale = dynamic_cast<PhaseScale*>( get_frame()->get_x_scale() );
  if (!scale)
    throw Error (InvalidState, "Pulsar::MultiPhase::get_scale",
		 "x scale is not a PhaseScale");
  return scale;
}
