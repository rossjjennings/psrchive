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

  import ( "x", PlotScale::Interface(), &PlotFrame::get_x_scale );
  import ( "y", PlotScale::Interface(), &PlotFrame::get_y_scale );

  import ( "x", PlotAxis::Interface(), &PlotFrame::get_x_axis );
  import ( "y", PlotAxis::Interface(), &PlotFrame::get_y_axis );

  add( &PlotFrame::get_character_height,
       &PlotFrame::set_character_height,
       "ch", "Character height" );

  add( &PlotFrame::get_character_font,
       &PlotFrame::set_character_font,
       "cf", "Character font" );

  add( &PlotFrame::get_line_width,
       &PlotFrame::set_line_width,
       "lw", "Line width" );

  add( &PlotFrame::get_publication_quality,
       &PlotFrame::set_publication_quality,
       "pub", "Publication quality" );

  import ( "above", PlotLabel::Interface(), &PlotFrame::get_label_above );
  import ( "below", PlotLabel::Interface(), &PlotFrame::get_label_below );

  add( &PlotFrame::get_label_offset,
       &PlotFrame::set_label_offset,
       "laboff", "Character height between label and frame" );

  add( &PlotFrame::get_label_spacing,
       &PlotFrame::set_label_spacing,
       "labsep", "Character height between label rows" );
}
