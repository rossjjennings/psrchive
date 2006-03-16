//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/StokesCylindrical.h,v $
   $Revision: 1.6 $
   $Date: 2006/03/16 17:07:17 $
   $Author: straten $ */

#ifndef __Pulsar_StokesCylindrical_h
#define __Pulsar_StokesCylindrical_h

#include "Pulsar/MultiPhase.h"
#include "Pulsar/StokesPlot.h"
#include "Pulsar/PosAngPlot.h"

namespace Pulsar {

  class PolnProfile;

  //! Plots a single pulse profile
  class StokesCylindrical : public MultiPhase {

  public:

    //! Default constructor
    StokesCylindrical ();

    //! Get the text interface to the configuration attributes
    TextInterface::Class* get_interface ();

    //! Text interface to the StokesCylindrical class
    class Interface : public TextInterface::To<StokesCylindrical> {
    public:
      Interface (StokesCylindrical* = 0);
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

  protected:

    StokesPlot flux;
    PosAngPlot orientation;
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
