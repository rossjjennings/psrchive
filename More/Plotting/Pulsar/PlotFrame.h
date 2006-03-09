//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotFrame.h,v $
   $Revision: 1.5 $
   $Date: 2006/03/09 23:02:12 $
   $Author: straten $ */

#ifndef __Pulsar_PlotFrame_h
#define __Pulsar_PlotFrame_h

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

    //! Get the x-axis
    PlotAxis* get_x_axis() { return &x_axis; }

    //! Get the x-axis
    PlotAxis* get_y_axis() { return &y_axis; }

    //! Get the above-frame label
    PlotLabel* get_label_above () { return &above; }

    //! Get the below-frame label
    PlotLabel* get_label_below () { return &below; }

    //! Decorate the frame
    virtual void decorate (const Archive*);

    //! Set the offset between labels and frame (multiple of character height)
    void set_label_offset (float offset) { label_offset = offset; }
    float get_label_offset () const { return label_offset; }

    //! Set the spacing between label rows (multiple of character height)
    void set_label_spacing (float spacing) { label_spacing = spacing; }
    float get_label_spacing () const { return label_spacing; }

    //! Get the text interface to the archive class
    ArchiveTI* get_interface (const Archive*);

  protected:

    PlotAxis x_axis;
    PlotAxis y_axis;
    PlotLabel above;
    PlotLabel below;
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
