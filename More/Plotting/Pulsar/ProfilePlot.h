//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/ProfilePlot.h,v $
   $Revision: 1.2 $
   $Date: 2006/03/07 23:13:23 $
   $Author: straten $ */

#ifndef __Pulsar_SinglePlotter_h
#define __Pulsar_SinglePlotter_h

#include "Pulsar/FluxPlotter.h"

namespace Pulsar {

  //! Plots a single pulse profile
  class SinglePlotter : public FluxPlotter {

    //! Default constructor
    SinglePlotter ();

    //! Load up the profiles
    void get_profiles (const Archive* data);

    //! Set the polarization to plot
    void set_pol (unsigned ipol);
    unsigned get_pol () const {return ipol;}
    
    //! Derived classes must draw in the current viewport
    void draw (const Archive*);

    //! draw the profile transitions in the current viewport and window
    void draw_transitions (const Profile* profile) const;

  protected:

    unsigned ipol;


  };

}

#endif
