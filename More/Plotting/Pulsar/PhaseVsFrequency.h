//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PhaseVsFrequency.h,v $
   $Revision: 1.7 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __Pulsar_PhaseVsFrequency_h
#define __Pulsar_PhaseVsFrequency_h

#include "Pulsar/PhaseVsPlot.h"
#include "Pulsar/PlotIndex.h"

namespace Pulsar {

  //! Plots images of something as a function of pulse phase vs something
  class PhaseVsFrequency : public PhaseVsPlot {

  public:

    //! Default constructor
    PhaseVsFrequency ();

    //! Get the text interface to the configuration attributes
    TextInterface::Class* get_interface ();

    //! Text interface to the PhaseVsFrequency class
    class Interface : public TextInterface::To<PhaseVsFrequency> {
    public:
      Interface (PhaseVsFrequency* = 0);
    };

    //! Set the minimum and maximum values on the y-axis
    void prepare (const Archive* data);

    //! Get the default label for the y axis
    std::string get_ylabel (const Archive*);

    //! Derived classes must provide the number of rows
    unsigned get_nrow (const Archive* data);

    //! Derived classes must provide the Profile for the specified row
    const Profile* get_Profile (const Archive* data, unsigned row);

    //! Set the sub-integration to plot (where applicable)
    void set_subint (const PlotIndex& _isubint) { isubint = _isubint; }
    PlotIndex get_subint () const { return isubint; }
    
    //! Set the polarization to plot
    void set_pol (const PlotIndex& _ipol) { ipol = _ipol; }
    PlotIndex get_pol () const { return ipol; }

  protected:

    PlotIndex isubint;
    PlotIndex ipol;

  };

}

#endif
