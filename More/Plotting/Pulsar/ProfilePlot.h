//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/ProfilePlot.h,v $
   $Revision: 1.3 $
   $Date: 2006/03/08 03:51:28 $
   $Author: straten $ */

#ifndef __Pulsar_SinglePlotter_h
#define __Pulsar_SinglePlotter_h

#include "Pulsar/FluxPlotter.h"

namespace Pulsar {

  //! Plots a single pulse profile
  class SinglePlotter : public FluxPlotter {

  public:

    //! Load the profiles
    void get_profiles (const Archive* data);

    //! Draw using FluxPlotter::draw, then maybe draw_transitions
    void draw (const Archive*);

    //! Draw the profile transitions in the current viewport and window
    void draw_transitions (const Profile* profile) const;

  };

}

#endif
