//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/Pulsar/PhaseVsMore.h

#ifndef __Pulsar_PhaseVsMore_h
#define __Pulsar_PhaseVsMore_h

#include "Pulsar/PhaseVsPlot.h"
#include "Pulsar/Profile.h"

namespace Pulsar {

  //! Plots images of something as a function of pulse phase vs something
  class PhaseVsMore : public PhaseVsPlot
  {
  public:

    //! Default constructor
    PhaseVsMore ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the PhaseVsMore class
    class Interface : public TextInterface::To<PhaseVsMore>
    {
    public:
      Interface (PhaseVsMore* = 0);
    };

    //! Get the default label for the y axis
    std::string get_ylabel (const Archive*);

    //! Derived classes must provide the number of rows
    unsigned get_nrow (const Archive* data);

    //! Derived classes must provide the Profile for the specified row
    const Profile* get_Profile (const Archive* data, unsigned row);

    //! Set the sub-integration to plot (where applicable)
    void set_subint (const unsigned& _isubint) { isubint = _isubint; }
    unsigned get_subint () const { return isubint; }
    
    //! Set the frequency channel to plot
    void set_chan (const unsigned& _ichan) { ichan = _ichan; }
    unsigned get_chan () const { return ichan; }

    //! Prepare for plotting
    void prepare (const Archive *data);

  protected:

    unsigned isubint;
    unsigned ichan;

    Reference::To<const Profile> profile;
  };

}

#endif
