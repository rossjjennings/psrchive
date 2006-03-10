//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/ProfilePlot.h,v $
   $Revision: 1.5 $
   $Date: 2006/03/10 20:37:58 $
   $Author: straten $ */

#ifndef __Pulsar_SinglePlotter_h
#define __Pulsar_SinglePlotter_h

#include "Pulsar/FluxPlotter.h"

namespace Pulsar {

  //! Plots a single pulse profile
  class SinglePlotter : public FluxPlotter {

  public:

    //! Default constructor
    SinglePlotter ();

    //! Get the text interface to the configuration attributes
    TextInterface::Class* get_interface ();

    //! Load the profiles
    void get_profiles (const Archive* data);

    //! Draw using FluxPlotter::draw, then maybe draw_transitions
    void draw (const Archive*);

    //! Set if cal transitions will be drawn
    void set_plot_cal_transitions (bool flag) { plot_cal_transitions = flag; }
    bool get_plot_cal_transitions () const { return plot_cal_transitions; }

    //! Draw the profile transitions in the current viewport and window
    void draw_transitions (const Profile* profile) const;

  protected:

    bool plot_cal_transitions;

  };

}

#endif
