//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PhaseVsTime.h,v $
   $Revision: 1.2 $
   $Date: 2006/03/10 20:37:58 $
   $Author: straten $ */

#ifndef __Pulsar_PhaseVsTime_h
#define __Pulsar_PhaseVsTime_h

#include "Pulsar/PhaseVsPlotter.h"

namespace Pulsar {

  //! Plots images of something as a function of pulse phase vs something
  class PhaseVsTime : public PhaseVsPlotter {

  public:

    //! Default constructor
    PhaseVsTime ();

    //! Get the text interface to the configuration attributes
    TextInterface::Class* get_interface ();

    //! Set the minimum and maximum values on the y-axis
    void prepare (const Archive* data);

    //! Get the default label for the y axis
    std::string get_ylabel (const Archive*);

    //! Derived classes must provide the number of rows
    unsigned get_nrow (const Archive* data);

    //! Derived classes must provide the Profile for the specified row
    const Profile* get_Profile (const Archive* data, unsigned row);

    //! Set the frequency channel to plot
    void set_chan (unsigned _ichan) { ichan = _ichan; }
    unsigned get_chan () const { return ichan; }
    
    //! Set the polarization to plot
    void set_pol (unsigned _ipol) { ipol = _ipol; }
    unsigned get_pol () const { return ipol; }

  protected:

    std::string time_string;
    unsigned ichan;
    unsigned ipol;

  };

}

#endif
