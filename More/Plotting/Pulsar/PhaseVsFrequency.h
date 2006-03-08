//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PhaseVsFrequency.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/08 22:33:29 $
   $Author: straten $ */

#ifndef __Pulsar_PhaseVsFrequency_h
#define __Pulsar_PhaseVsFrequency_h

#include "Pulsar/PhaseVsPlotter.h"

namespace Pulsar {

  //! Plots images of something as a function of pulse phase vs something
  class PhaseVsFrequency : public PhaseVsPlotter {

  public:

    //! Default constructor
    PhaseVsFrequency ();

    //! Set the minimum and maximum values on the y-axis
    void prepare (const Archive* data);

    //! Get the default label for the y axis
    std::string get_ylabel (const Archive*);

    //! Derived classes must provide the number of rows
    unsigned get_nrow (const Archive* data);

    //! Derived classes must provide the Profile for the specified row
    const Profile* get_Profile (const Archive* data, unsigned row);

    //! Set the sub-integration to plot (where applicable)
    void set_subint (unsigned _isubint) { isubint = _isubint; }
    unsigned get_subint () const { return isubint; }
    
    //! Set the polarization to plot
    void set_pol (unsigned _ipol) { ipol = _ipol; }
    unsigned get_pol () const { return ipol; }

  protected:

    unsigned isubint;
    unsigned ipol;

  };

}

#endif
