//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2024 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/pgutil/ColourBar.h

#ifndef __ColourBar_H
#define __ColourBar_H

#include "TextInterface.h"
#include <string>

namespace pgplot {

  //! Colour bar (wedge) manager
  class ColourBar : public Reference::Able
  {
    /*
    SIDE   (input)  : The first character must be one of the characters
                      'B', 'L', 'T', or 'R' signifying the Bottom, Left,
                      Top, or Right edge of the viewport.
                      The second character should be 'I' to use PGIMAG
                      to draw the wedge, or 'G' to use PGGRAY.
    */
    char side = 'N';

    /*
    DISP   (input)  : the displacement of the wedge from the specified
                      edge of the viewport, measured outwards from the
                      viewport in units of the character height. Use a
                      negative value to write inside the viewport, a
                      positive value to write outside.
    */
    float displacement = 4;

    /*    
    WIDTH  (input)  : The total width of the wedge including annotation,
                      in units of the character height.
    */
    float width = 3;

    /*
    LABEL  (input)  : Optional units label. If no label is required
                      use ' '.
    */
    std::string label = " ";
  
  public:

    //! Set the colour map name
    void set_label (std::string _label) { label = _label; }
    std::string get_label () const { return label; }

    //! Set the side
    void set_side (char _side);
    char get_side () const { return side; }

    //! Set the total width of the wedge including annotation, in units of the character height.
    void set_width (float _width) { width = _width; }
    float get_width () const { return width; }

    //! Set displacement of the wedge from the specified edge of the viewport
    /*! Displacement is measured outwards from the viewport in units of the character height. 
        Use a negative value to write inside the viewport, a positive value to write outside.
    */
    void set_displacement (float _displacement) { displacement = _displacement; }
    float get_displacement () const { return displacement; }

    //! Plot the colour bar using the current attributes
    /*
    @ param foreground: The value which is to appear with shade 1
    @ param background: The value which is to appear with shade 0
    @ param pgimag: If true, use PGIMAG; if false, use PGGRAY
    Use the values of foreground and background that were supplied to PGGRAY or PGIMAG.
    */
    void plot (float foreground, float background, bool pgimag=true);

    //! The text interface to this class
    class Interface;
  };

  class ColourBar::Interface : public TextInterface::To<ColourBar> {
  public:
    Interface (ColourBar* = 0);
  };

}
#endif
