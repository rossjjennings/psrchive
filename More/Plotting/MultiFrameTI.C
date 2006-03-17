/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/MultiFrame.h"
#include "Pulsar/PlotFrameSize.h"
#include "Pulsar/PlotScale.h"

Pulsar::MultiFrame::Interface::Interface (MultiFrame* instance)
{
  if (instance)
    set_instance (instance);

  if (instance->has_shared_x_scale())
    import ( "x", PlotScale::Interface(), &MultiFrame::get_shared_x_scale );

  if (instance->has_shared_y_scale())
    import ( "y", PlotScale::Interface(), &MultiFrame::get_shared_y_scale );

  import_filter = true;

  import ( "", std::string(), PlotFrameSize::Interface(), &MultiFrame::get_frame );
}
