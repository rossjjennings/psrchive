//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotFrame.h,v $
   $Revision: 1.7 $
   $Date: 2006/03/11 03:35:17 $
   $Author: straten $ */

#ifndef __Pulsar_PlotFrame_h
#define __Pulsar_PlotFrame_h

#include "Pulsar/PlotZoom.h"
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

    //! Get the x-zoom
    PlotZoom* get_x_zoom() { return x_zoom; }
    //! Set the x-zoom
    void set_x_zoom (PlotZoom* zoom) { x_zoom = zoom; }

    //! Get the x-axis
    PlotAxis* get_x_axis() { return x_axis; }

    //! Get the x-zoom
    PlotZoom* get_y_zoom() { return y_zoom; }
    //! Set the y-zoom
    void set_y_zoom (PlotZoom* zoom) { y_zoom = zoom; }

    //! Get the x-axis
    PlotAxis* get_y_axis() { return y_axis; }

    //! Get the above-frame label
    PlotLabel* get_label_above () { return above; }

    //! Get the below-frame label
    PlotLabel* get_label_below () { return below; }

    //! Decorate the frame
    virtual void decorate (const Archive*);

    //! Focus the frame
    /*! By default a frame exists in the current viewport */
    virtual void focus () { }

    //! Set the offset between labels and frame (multiple of character height)
    void set_label_offset (float offset) { label_offset = offset; }
    float get_label_offset () const { return label_offset; }

    //! Set the spacing between label rows (multiple of character height)
    void set_label_spacing (float spacing) { label_spacing = spacing; }
    float get_label_spacing () const { return label_spacing; }

    //! Get the text interface to the archive class
    ArchiveTI* get_interface (const Archive*);

  protected:

    Reference::To<PlotZoom> x_zoom;
    Reference::To<PlotZoom> y_zoom;

    Reference::To<PlotAxis> x_axis;
    Reference::To<PlotAxis> y_axis;

    Reference::To<PlotLabel> above;
    Reference::To<PlotLabel> below;

    float label_offset;
    float label_spacing;

    //! Plot the label; direction=+/-1 for above/below frame
    void decorate (const Archive*, PlotLabel*, float direction);
    //! Plot the label; side=0/.5/1 for left/centre/right
    void decorate (const Archive*, const std::string& label, 
		   float side, float direction);
    //! Replace any '=name' with 'value'
    void resolve_variables (const Archive* data, std::string& label);

  private:

    Reference::To<ArchiveTI> archive_interface;

  };

}

#endif
