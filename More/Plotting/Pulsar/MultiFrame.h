//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/MultiFrame.h,v $
   $Revision: 1.12 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __Pulsar_MultiFrame_h
#define __Pulsar_MultiFrame_h

#include "Pulsar/PlotAttributes.h"
#include "Pulsar/PlotFrameSize.h"
#include <map>

namespace Pulsar {

  //! Manages multiple plot frames
  class MultiFrame : public PlotAttributes {

  public:

    //! Text interface to the MultiFrame class
    class Interface : public TextInterface::To<MultiFrame> {
    public:
      Interface (MultiFrame* = 0);
    };

    //! Construct a new PlotFrameSize from the given PlotFrame and map it
    PlotFrameSize* manage (const std::string& name, PlotFrame*);

    //! Return a previously mapped plot frame
    PlotFrameSize* get_frame (const std::string& name);

    //! Return the shared x-scale
    PlotScale* get_shared_x_scale() { return x_scale; }
    bool has_shared_x_scale () { return x_scale; }
    void set_shared_x_scale (PlotScale* x) { x_scale = x; }

    //! Return the shared y-scale
    PlotScale* get_shared_y_scale() { return y_scale; }
    bool has_shared_y_scale () { return y_scale; }
    void set_shared_y_scale (PlotScale* y) { y_scale = y; }

    //! Set the character height
    void set_character_height (float height);

    //! Set the character font
    void set_character_font (int font);

    //! Set the line width
    void set_line_width (int width);
 
    //! Set publication quality character height and font and line width
    void set_publication_quality (bool flag = true);

  protected:

    std::map< std::string, Reference::To<PlotFrameSize> > frames;

    Reference::To<PlotScale> x_scale;
    Reference::To<PlotScale> y_scale;

  };

}

#endif
