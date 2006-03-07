//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotFrame.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/07 23:13:23 $
   $Author: straten $ */

#ifndef __Pulsar_PlotFrame_h
#define __Pulsar_PlotFrame_h

#include "Pulsar/PlotAxis.h"
#include "Pulsar/PlotLabel.h"

namespace Pulsar {

  //! Plots the position angle of the linearly polarized radiation
  class PlotFrame : public Reference::Able {

  public:

    //! Default constructor
    PlotFrame ();

    //! Get the x-axis
    PlotAxis* get_x_axis() { return &x_axis; }

    //! Get the x-axis
    PlotAxis* get_y_axis() { return &y_axis; }

    //! Get the over-frame label
    PlotLabel* get_label_over () { return &over; }

    //! Get the under-frame label
    PlotLabel* get_label_under () { return &under; }

  protected:

    PlotAxis x_axis;
    PlotAxis y_axis;
    PlotLabel over;
    PlotLabel under;

  };

}

#endif
