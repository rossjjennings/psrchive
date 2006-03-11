//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PhaseVsPlot.h,v $
   $Revision: 1.5 $
   $Date: 2006/03/11 21:56:54 $
   $Author: straten $ */

#ifndef __Pulsar_PhaseVsPlotter_h
#define __Pulsar_PhaseVsPlotter_h

#include "Pulsar/PhasePlot.h"

namespace Pulsar {

  class Profile;

  //! Plots images of something as a function of pulse phase vs something
  class PhaseVsPlotter : public PhasePlot {

  public:

    //! Default constructor
    PhaseVsPlotter ();

    //! Get the text interface to the configuration attributes
    TextInterface::Class* get_interface ();

    //! Derived classes must provide the number of rows
    virtual unsigned get_nrow (const Archive* data) = 0;

    //! Derived classes must provide the Profile for the specified row
    virtual const Profile* get_Profile (const Archive* data, unsigned row) = 0;

    //! Draw in the current viewport
    void draw (const Archive*);

    //! Provide access to the z-zoom
    PlotZoom* get_z_zoom () { return &z_zoom; }

  protected:

    PlotZoom z_zoom;

  };

}

#endif
