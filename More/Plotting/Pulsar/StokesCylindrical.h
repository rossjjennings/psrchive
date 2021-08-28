//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/Pulsar/StokesCylindrical.h

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
    TextInterface::Parser* get_interface ();

    // Text interface to the StokesCylindrical class
    class Interface : public TextInterface::To<StokesCylindrical> {
    public:
      Interface (StokesCylindrical* = 0);
    };

    //! Ensure that frames are properly initialized
    void prepare (const Archive*);

    //! Set the sub-integration to plot (where applicable)
    void set_subint (const Index& _isubint) { isubint = _isubint; }
    Index get_subint () const { return isubint; }
    
    //! Set the frequency channel to plot (where applicable)
    void set_chan (const Index& _ichan) { ichan = _ichan; }
    Index get_chan () const { return ichan; }

    //! Get the StokesPlot
    StokesPlot* get_flux () { return &flux; }

    //! Get the PosAngPlot
    PosAngPlot* get_orientation () { return &orientation; }

    //! Set the name of the file to which comma-separated values are written
    void set_csv_filename (const std::string& fname) { csv_filename = fname; }
    const std::string& get_csv_filename () const { return csv_filename; }

  protected:

    StokesPlot flux;
    PosAngPlot orientation;
    Index isubint;
    Index ichan;

    std::string csv_filename;

    template<class T> void prepare (T* plot, const Archive* data = 0)
    {
      plot->set_subint (isubint);
      plot->set_chan (ichan);
      if (data)
        plot->prepare (data);
    }

  };

}

#endif
