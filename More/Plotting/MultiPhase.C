#include "Pulsar/MultiPhase.h"

Pulsar::MultiPhase::MultiPhase ()
{
  get_frame()->set_x_zoom( new PhaseScale );
  frames.set_shared_x_zoom (get_frame()->get_x_zoom());
}

//! Manage a plot
void Pulsar::MultiPhase::manage (const std::string& name, Plot* plot)
{
  plot->get_frame()->set_x_zoom( frames.get_shared_x_zoom() );
  MultiPlot::manage (name, plot);
}

//! Get the scale
Pulsar::PhaseScale* Pulsar::MultiPhase::get_scale ()
{
  PhaseScale* scale = dynamic_cast<PhaseScale*>( frames.get_shared_x_zoom() );
  if (!scale)
    throw Error (InvalidState, "Pulsar::MultiPhase::get_scale",
		 "x scale is not a PhaseScale");
  return scale;
}
