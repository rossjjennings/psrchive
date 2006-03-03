//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/ProfilePlot.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/03 23:23:36 $
   $Author: straten $ */

#ifndef __Pulsar_SinglePlotter_h
#define __Pulsar_SinglePlotter_h

#include "Pulsar/ProfilePlotter.h"

namespace Pulsar {

  class Profile;

  //! Plots a single pulse profile

  class SinglePlotter : public ProfilePlotter {

    //! Derived classes must compute the minimum and maximum values (y-axis)
    void minmax (const Archive*, float& min, float& max);

    //! Derived classes must draw in the current viewport
    void draw (const Archive*);

    //! draw the profile in the current viewport and window
    void draw (const Profile* profile) const;

    //! draw the profile transitions in the current viewport and window
    void draw_transitions (const Profile* profile) const;

  };

}

#endif
