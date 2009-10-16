/***************************************************************************
 *
 *   Copyright (C) 2006-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PlotFrame.h"
#include "Pulsar/PlotScale.h"
#include "Pulsar/PlotAxis.h"
#include "Pulsar/PlotLabel.h"

Pulsar::PlotFrame::Interface::Interface (PlotFrame* instance)
{
  if (!instance)
    throw Error (InvalidState, "Pulsar::PlotFrame::Interface",
		 "instance not set");

  set_instance (instance);

  insert ( "x", instance->get_x_scale()->get_interface() );
  insert ( "y", instance->get_y_scale()->get_interface() );

  import ( "x", PlotAxis::Interface(), 
      (PlotAxis* (PlotFrame::*)(void))&PlotFrame::get_x_axis );
  import ( "y", PlotAxis::Interface(),
      (PlotAxis* (PlotFrame::*)(void))&PlotFrame::get_y_axis );

  import ( PlotAttributes::Interface() );

  import ( "below", PlotLabel::Interface(), &PlotFrame::get_label_below );

}
