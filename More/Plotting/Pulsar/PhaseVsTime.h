//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/Pulsar/PhaseVsTime.h

#ifndef __Pulsar_PhaseVsTime_h
#define __Pulsar_PhaseVsTime_h

#include "Pulsar/PhaseVsPlot.h"
#include "Pulsar/TimeScale.h"
#include "Pulsar/Index.h"

namespace Pulsar {

  //! Plots images of something as a function of pulse phase vs something
  class PhaseVsTime : public PhaseVsPlot {

  public:

    //! Default constructor
    PhaseVsTime ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the PhaseVsTime class
    class Interface : public TextInterface::To<PhaseVsTime> {
    public:
      Interface (PhaseVsTime* = 0);
    };

    //! Derived classes must provide the Profile for the specified row
    const Profile* get_Profile (const Archive* data, unsigned row);

    //! Return pointer to y scale
    TimeScale* get_y_scale () { return y_scale; }

    //! Set the frequency channel to plot
    void set_chan (const Index& _ichan) { ichan = _ichan; }
    Index get_chan () const { return ichan; }
    
    //! Set the polarization to plot
    void set_pol (const Index& _ipol) { ipol = _ipol; }
    Index get_pol () const { return ipol; }

  protected:

    Index ichan;
    Index ipol;
    Reference::To<TimeScale> y_scale;

  };

}

#endif
