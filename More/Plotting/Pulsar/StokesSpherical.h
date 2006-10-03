//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/StokesSpherical.h,v $
   $Revision: 1.7 $
   $Date: 2006/10/03 21:10:03 $
   $Author: straten $ */

#ifndef __Pulsar_StokesSpherical_h
#define __Pulsar_StokesSpherical_h

#include "Pulsar/MultiPhase.h"
#include "Pulsar/StokesPlot.h"
#include "Pulsar/PosAngPlot.h"
#include "Pulsar/EllAngPlot.h"

namespace Pulsar {

  //! Plots a single pulse profile
  class StokesSpherical : public MultiPhase {

  public:

    //! Default constructor
    StokesSpherical ();

    //! Get the text interface to the configuration attributes
    TextInterface::Class* get_interface ();

    //! Text interface to the StokesSpherical class
    class Interface : public TextInterface::To<StokesSpherical> {
    public:
      Interface (StokesSpherical* = 0);
    };

    //! Ensure that frames are properly initialized
    void prepare (const Archive*);

    //! Set the sub-integration to plot (where applicable)
    void set_subint (const PlotIndex& _isubint) { isubint = _isubint; }
    PlotIndex get_subint () const { return isubint; }
    
    //! Set the frequency channel to plot (where applicable)
    void set_chan (const PlotIndex& _ichan) { ichan = _ichan; }
    PlotIndex get_chan () const { return ichan; }

    //! Get the StokesPlot
    StokesPlot* get_flux () { return &flux; }

    //! Get the PosAngPlot
    PosAngPlot* get_orientation () { return &orientation; }

    //! Get the EllAngPlot
    EllAngPlot* get_ellipticity () { return &ellipticity; }

  protected:

    StokesPlot flux;
    PosAngPlot orientation;
    EllAngPlot ellipticity;
    PlotIndex isubint;
    PlotIndex ichan;

    template<class T> void prepare (T* plot)
    {
      plot->set_subint (isubint);
      plot->set_chan (ichan);
    }

  };

}

#endif
