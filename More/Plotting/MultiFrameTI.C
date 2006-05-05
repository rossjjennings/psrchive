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

  add( &MultiFrame::get_character_height,
       &MultiFrame::set_character_height,
       "ch", "Character height" );

  add( &MultiFrame::get_character_font,
       &MultiFrame::set_character_font,
       "cf", "Character font" );

  add( &MultiFrame::get_line_width,
       &MultiFrame::set_line_width,
       "lw", "Line width" );

  add( &MultiFrame::get_publication_quality,
       &MultiFrame::set_publication_quality,
       "pub", "Publication quality" );

  import_filter = true;

  import ( "", std::string(), PlotFrameSize::Interface(),
	   &MultiFrame::get_frame );
}
