//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PhaseVsPlot.h,v $
   $Revision: 1.2 $
   $Date: 2006/03/09 22:29:53 $
   $Author: straten $ */

#ifndef __Pulsar_PhaseVsPlotter_h
#define __Pulsar_PhaseVsPlotter_h

#include "Pulsar/ProfilePlotter.h"

namespace Pulsar {

  class Profile;

  //! Plots images of something as a function of pulse phase vs something
  class PhaseVsPlotter : public ProfilePlotter {

  public:

    //! Default constructor
    PhaseVsPlotter ();

    //! Derived classes must provide the number of rows
    virtual unsigned get_nrow (const Archive* data) = 0;

    //! Derived classes must provide the Profile for the specified row
    virtual const Profile* get_Profile (const Archive* data, unsigned row) = 0;

    //! Draw in the current viewport
    void draw (const Archive*);

    //! Return the text interface
    TextInterface::Class* get_interface ();

    //! Provide access to the z-axis
    PlotAxis* get_z_axis () { return &z_axis; }

  protected:

    PlotAxis z_axis;

  };

}

#endif
