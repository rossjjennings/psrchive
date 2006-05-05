//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotFrame.h,v $
   $Revision: 1.16 $
   $Date: 2006/05/05 02:12:57 $
   $Author: straten $ */

#ifndef __Pulsar_PlotFrame_h
#define __Pulsar_PlotFrame_h

#include "Pulsar/PlotScale.h"
#include "Pulsar/PlotAxis.h"
#include "Pulsar/PlotLabel.h"

#include <string>

namespace Pulsar {

  class Archive;
  class ArchiveTI;

  //! Stores the properties of the plot frame
  class PlotFrame : public Reference::Able {

  public:

    //! Default constructor
    PlotFrame ();

    //! Destructor
    ~PlotFrame ();

    //! Text interface to the PlotFrame class
    class Interface : public TextInterface::To<PlotFrame> {
    public:
      Interface (PlotFrame* = 0);
    };

    //! Get the x-scale
    PlotScale* get_x_scale (bool allow_transpose = false);
    //! Set the x-scale
    void set_x_scale (PlotScale* scale);

    //! Get the x-axis
    PlotAxis* get_x_axis (bool allow_transpose = false);

    //! Get the x-scale
    PlotScale* get_y_scale (bool allow_transpose = false);
    //! Set the y-scale
    void set_y_scale (PlotScale* scale);

    //! Get the x-axis
    PlotAxis* get_y_axis (bool allow_transpose = false);

    //! Set the character height
    void set_character_height (float height) { character_height = height; }
    float get_character_height () const { return character_height; }

    //! Set the character font
    void set_character_font (int font) { character_font = font; }
    int get_character_font () const { return character_font; }

    //! Set the line width
    void set_line_width (int width) { line_width = width; }
    int get_line_width () const { return line_width; }

    //! Set publication quality character height and font and line width
    virtual void set_publication_quality (bool flag = true);
    bool get_publication_quality () const { return false; }

    //! Get the above-frame label
    PlotLabel* get_label_above () { return above; }

    //! Get the below-frame label
    PlotLabel* get_label_below () { return below; }

    //! Set the offset between labels and frame (multiple of character height)
    void set_label_offset (float offset) { label_offset = offset; }
    float get_label_offset () const { return label_offset; }

    //! Set the spacing between label rows (multiple of character height)
    void set_label_spacing (float spacing) { label_spacing = spacing; }
    float get_label_spacing () const { return label_spacing; }

    //! transpose the x and y axes
    void set_transpose (bool flag = true) { transpose = flag; }
    bool get_transpose () const { return transpose; }
 
    //! Draw the axes of the frame
    virtual void draw_axes (const Archive*);

    //! Label the axes os the frame
    virtual void label_axes (const std::string& default_x,
			     const std::string& default_y);

    //! Decorate the frame
    virtual void decorate (const Archive*);

    //! Focus the frame
    /*! By default a frame exists in the current viewport */
    virtual void focus (const Archive*);

    //! Get the text interface to the archive class
    ArchiveTI* get_interface (const Archive*);

  protected:

    Reference::To<PlotScale> x_scale;
    Reference::To<PlotScale> y_scale;

    Reference::To<PlotAxis> x_axis;
    Reference::To<PlotAxis> y_axis;

    float character_height;
    int character_font;
    int line_width;

    Reference::To<PlotLabel> above;
    Reference::To<PlotLabel> below;

    float label_offset;
    float label_spacing;
    bool  transpose;

    //! Plot the label; direction=+/-1 for above/below frame
    void decorate (const Archive*, PlotLabel*, float direction);
    //! Plot the label; side=0/.5/1 for left/centre/right
    void decorate (const Archive*, const std::string& label, 
		   float side, float direction);

  private:

    Reference::To<ArchiveTI> archive_interface;

  };

}

#endif
