/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/MultiPhase.h"

Pulsar::MultiPhase::MultiPhase ()
{
  get_frame()->set_x_scale( new PhaseScale );
  frames.set_shared_x_scale (get_frame()->get_x_scale());
}

//! Manage a plot
void Pulsar::MultiPhase::manage (const std::string& name, Plot* plot)
{
  plot->get_frame()->set_x_scale( frames.get_shared_x_scale() );
  MultiPlot::manage (name, plot);
}

//! Get the scale
Pulsar::PhaseScale* Pulsar::MultiPhase::get_scale ()
{
  PhaseScale* scale = dynamic_cast<PhaseScale*>( frames.get_shared_x_scale() );
  if (!scale)
    throw Error (InvalidState, "Pulsar::MultiPhase::get_scale",
		 "x scale is not a PhaseScale");
  return scale;
}
