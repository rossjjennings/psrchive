//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/Pulsar/PhaseVsFrequency.h

#ifndef __Pulsar_PhaseVsFrequency_h
#define __Pulsar_PhaseVsFrequency_h

#include "Pulsar/PhaseVsPlot.h"
#include "Pulsar/Index.h"
#include "Pulsar/Integration.h"

namespace Pulsar {

  //! Plots images of something as a function of pulse phase vs something
  class PhaseVsFrequency : public PhaseVsPlot {

  public:

    //! Default constructor
    PhaseVsFrequency ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the PhaseVsFrequency class
    class Interface : public TextInterface::To<PhaseVsFrequency> {
    public:
      Interface (PhaseVsFrequency* = 0);
    };

    //! Get the default label for the y axis
    std::string get_ylabel (const Archive*);

    //! Derived classes must provide the Profile for the specified row
    const Profile* get_Profile (const Archive* data, unsigned row);

    //! Set the sub-integration to plot (where applicable)
    void set_subint (const Index& _isubint) { isubint = _isubint; }
    Index get_subint () const { return isubint; }
    
    //! Set the polarization to plot
    void set_pol (const Index& _ipol) { ipol = _ipol; }
    Index get_pol () const { return ipol; }

    //! Prepare for plotting
    void prepare (const Archive *data);

  protected:

    Index isubint;
    Index ipol;

    Reference::To<const Integration> subint;

  };

}

#endif
