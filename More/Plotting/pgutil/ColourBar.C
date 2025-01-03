/***************************************************************************
 *
 *   Copyright (C) 2024 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "ColourBar.h"
#include <cpgplot.h>

void pgplot::ColourBar::plot (float fg, float bg, bool pgimag)
{
  if (side == 'N')
    return;

  char side_str[3];
  side_str[0] = side;
  side_str[1] = (pgimag) ? 'I' : 'G';
  side_str[2] = '\0';

  // PGWEDG(SIDE, DISP, WIDTH, FG, BG, LABEL)
  cpgwedg (side_str, displacement, width, fg, bg, label.c_str());
}

void pgplot::ColourBar::set_side (char _side)
{ 
  switch (_side)
  {
    case 'B':
    case 'L':
    case 'T':
    case 'R':
    case 'N':
      side = _side; 
      break;
    default:
      throw Error (InvalidParam, "pgplot::ColourBar::set_side", "invalid code=%c", _side);
  }
}

pgplot::ColourBar::Interface::Interface (ColourBar* instance)
{
  if (instance)
    set_instance (instance);

  add( &ColourBar::get_side,
       &ColourBar::set_side,
       "side", "B,L,T,R,N = Bottom, Left, Top, Right, None" );

  add( &ColourBar::get_label,
       &ColourBar::set_label,
       "label", "Label on colour bar" );

  add( &ColourBar::get_displacement,
       &ColourBar::set_displacement,
       "offset", "Outward offset from plot edge to colour bar" );

  add( &ColourBar::get_width,
       &ColourBar::set_width,
       "width", "Width of colour bar" );
}
