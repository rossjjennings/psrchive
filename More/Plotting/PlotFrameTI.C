/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PlotFrame.h"
#include "Pulsar/PlotScale.h"
#include "Pulsar/PlotAxis.h"
#include "Pulsar/PlotLabel.h"

Pulsar::PlotFrame::Interface::Interface (PlotFrame* instance)
{
  if (instance)
    set_instance (instance);

  import ( "x", PlotScale::Interface(), 
      (PlotScale* (PlotFrame::*)(void))&PlotFrame::get_x_scale );
  import ( "y", PlotScale::Interface(), 
      (PlotScale* (PlotFrame::*)(void))&PlotFrame::get_y_scale );

  import ( "x", PlotAxis::Interface(), 
      (PlotAxis* (PlotFrame::*)(void))&PlotFrame::get_x_axis );
  import ( "y", PlotAxis::Interface(),
      (PlotAxis* (PlotFrame::*)(void))&PlotFrame::get_y_axis );

  import ( PlotAttributes::Interface() );

  import ( "below", PlotLabel::Interface(), &PlotFrame::get_label_below );

}
