//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotFrame.h,v $
   $Revision: 1.3 $
   $Date: 2006/03/08 12:47:18 $
   $Author: straten $ */

#ifndef __Pulsar_PlotFrame_h
#define __Pulsar_PlotFrame_h

#include "Pulsar/PlotAxis.h"
#include "Pulsar/PlotLabel.h"

#include <string>

namespace Pulsar {

  class Archive;
  class ArchiveTI;

  //! Plots the position angle of the linearly polarized radiation
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

    //! Get the over-frame label
    PlotLabel* get_label_over () { return &over; }

    //! Get the under-frame label
    PlotLabel* get_label_under () { return &under; }

    //! Decorate the frame
    virtual void decorate (const Archive*);

    //! Get the text interface to the archive class
    ArchiveTI* get_interface (const Archive*);

  protected:

    PlotAxis x_axis;
    PlotAxis y_axis;
    PlotLabel over;
    PlotLabel under;

    //! Plot the label; direction=+/-1 for over/under frame
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
