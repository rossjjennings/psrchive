//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PosAngPlot.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/05 00:55:20 $
   $Author: straten $ */

#ifndef __Pulsar_PosAngPlotter_h
#define __Pulsar_PosAngPlotter_h

#include "Pulsar/ProfilePlotter.h"

namespace Pulsar {

  //! Plots the position angle of the linearly polarized radiation
  class PosAngPlotter : public ProfilePlotter {

  public:

    //! Default constructor
    PosAngPlotter ();

    //! return the minimum and maximum value in degrees
    void minmax (const Archive*, float& min, float& max);

    //! draw the position angle as a function of pulse phase
    void draw (const Archive*);

    std::string get_flux_label (const Archive* data);

    //! will draw the error bar on each point
    void set_error_bars (bool bars) { error_bars = bars; }

    //! will draw only those points with linear > threshold * sigma
    void set_threshold (float t) { threshold = t; }

    //! set the minimum position angle in degrees
    void set_min (float degrees) { deg_min = degrees; }

    //! set the maximum position angle in degrees
    void set_max (float degrees) { deg_max = degrees; }

  protected:

    //! Draw error bars
    bool error_bars;

    //! Noise threshold
    float threshold;

    //! Maximum degrees to plot
    float deg_max;

    //! Minimum degrees to plot
    float deg_min;

  };

}

#endif
