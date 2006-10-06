//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/Poincare.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __Pulsar_Poincare_h
#define __Pulsar_Poincare_h

#include "Pulsar/Plot.h"
#include "Pulsar/PlotIndex.h"
#include "Pulsar/PlotScale.h"

namespace Pulsar {

  //! Plots the Stokes vector in three-dimensional Poincare space
  class Poincare : public Plot {

  public:

    //! Default constructor
    Poincare ();

    //! Plot in the current viewport
    void plot (const Archive*);

    //! Text interface to the Poincare class
    class Interface : public TextInterface::To<Poincare> {
    public:
      Interface (Poincare* = 0);
    };

    //! Get the text interface to the configuration attributes
    TextInterface::Class* get_interface ()
    { return new Interface (this); }

    //! Get the text interface to the frame attributes
    //TextInterface::Class* get_frame_interface ();

    //! Set the longitude of the camera
    void set_longitude (float f) { longitude = f; }
    //! Get the longitude of the camera
    float get_longitude () const { return longitude; }

    //! Set the latitude of the camera
    void set_latitude (float f) { latitude = f; }
    //! Get the latitude of the camera
    float get_latitude () const { return latitude; }

    //! Set the sub-integration to plot (where applicable)
    void set_subint (PlotIndex _isubint) { isubint = _isubint; }
    PlotIndex get_subint () const { return isubint; }
    
    //! Set the frequency channel to plot (where applicable)
    void set_chan (PlotIndex _ichan) { ichan = _ichan; }
    PlotIndex get_chan () const { return ichan; }

    //! Provide access to the phase scale
    PlotScale* get_phase_scale () { return &phase_scale; }

    //! Set the animate of the camera
    void set_animate (bool f) { animate = f; }
    //! Get the animate of the camera
    bool get_animate () const { return animate; }

  protected:

    PlotIndex isubint;
    PlotIndex ichan;
    PlotScale phase_scale;

    float longitude;
    float latitude;
    bool animate;
  };

}

#endif
