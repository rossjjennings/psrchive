//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PosAngPlot.h,v $
   $Revision: 1.2 $
   $Date: 2006/03/06 12:59:42 $
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
    bool get_error_bars () const { return error_bars; }

    //! will draw only those points with linear > threshold * sigma
    void set_threshold (float t) { threshold = t; }
    float get_threshold () const { return threshold; }

    //! set the minimum position angle in degrees
    void set_min (float degrees) { deg_min = degrees; }
    float get_min () const { return deg_min; }

    //! set the maximum position angle in degrees
    void set_max (float degrees) { deg_max = degrees; }
    float get_max () const { return deg_max; }

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
