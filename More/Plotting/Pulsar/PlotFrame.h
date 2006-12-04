//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotFrame.h,v $
   $Revision: 1.23 $
   $Date: 2006/12/04 17:09:54 $
   $Author: straten $ */

#ifndef __Pulsar_PlotFrame_h
#define __Pulsar_PlotFrame_h

#include "Pulsar/PlotAttributes.h"
#include "Pulsar/PlotScale.h"
#include "Pulsar/PlotAxis.h"
#include "Pulsar/PlotLabel.h"

#include <string>

namespace Pulsar {

  class Archive;

  //! Stores the properties of the plot frame
  class PlotFrame : public PlotAttributes {

  public:

    //! Default constructor
    PlotFrame ();

    //! Destructor
    ~PlotFrame ();

    // Text interface to the PlotFrame class
    class Interface : public TextInterface::To<PlotFrame> {
    public:
      Interface (PlotFrame* = 0);
    };

    //! Set the viewport (normalized device coordinates)
    void set_viewport (float x0, float x1, float y0, float y1);

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

    //! Remove labels from above the plot
    void publication_quality ();

    //! Remove all labels
    void no_labels ();

    //! Get the above-frame label
    PlotLabel* get_label_above () { return above; }
    //! Get the above-frame label
    void set_label_above (PlotLabel*);

    //! Get the below-frame label
    PlotLabel* get_label_below () { return below; }
    //! Get the below-frame label
    void set_label_below (PlotLabel*);

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

  protected:

    Reference::To<PlotScale> x_scale;
    Reference::To<PlotScale> y_scale;

    Reference::To<PlotAxis> x_axis;
    Reference::To<PlotAxis> y_axis;

    Reference::To<PlotLabel> above;
    Reference::To<PlotLabel> below;

    bool  transpose;

  };

}

#endif
