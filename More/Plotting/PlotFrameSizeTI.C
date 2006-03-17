/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PlotFrameSize.h"
#include "Pulsar/PlotFrame.h"
#include "iopair.h"

Pulsar::PlotFrameSize::Interface::Interface (PlotFrameSize* instance)
{
  if (instance)
    set_instance (instance);

  import ( PlotFrame::Interface() );

  add( &PlotFrameSize::get_x_range,
       &PlotFrameSize::set_x_range,
       "vpx", "Normalized coordinates of viewport x-axis" );

  add( &PlotFrameSize::get_y_range,
       &PlotFrameSize::set_y_range,
       "vpy", "Normalized coordinates of viewport y-axis" );
}
