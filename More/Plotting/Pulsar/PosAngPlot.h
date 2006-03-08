//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PosAngPlot.h,v $
   $Revision: 1.3 $
   $Date: 2006/03/08 03:51:43 $
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
    void prepare (const Archive*);

    //! draw the position angle as a function of pulse phase
    void draw (const Archive*);

    std::string get_flux_label (const Archive* data);

    //! will draw the error bar on each point
    void set_error_bars (bool bars) { error_bars = bars; }
    bool get_error_bars () const { return error_bars; }

    //! will draw only those points with linear > threshold * sigma
    void set_threshold (float t) { threshold = t; }
    float get_threshold () const { return threshold; }

    //! Set the sub-integration to plot (where applicable)
    void set_subint (unsigned _isubint) { isubint = _isubint; }
    unsigned get_subint () const { return isubint; }
    
    //! Set the frequency channel to plot (where applicable)
    void set_chan (unsigned _ichan) { ichan = _ichan; }
    unsigned get_chan () const { return ichan; }

  protected:

    //! Draw error bars
    bool error_bars;

    //! Noise threshold
    float threshold;

    unsigned ichan;
    unsigned isubint;
  };

}

#endif
