//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/AnglePlot.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/09 18:10:46 $
   $Author: straten $ */

#ifndef __Pulsar_AnglePlotter_h
#define __Pulsar_AnglePlotter_h

#include "Pulsar/ProfilePlotter.h"
#include "Estimate.h"

namespace Pulsar {

  //! Plots the position angle of the linearly polarized radiation
  class AnglePlotter : public ProfilePlotter {

  public:

    //! Default constructor
    AnglePlotter ();

    //! Derived types compute the angles to be plotted
    virtual void get_angles (const Archive*,
			     std::vector< Estimate<double> >& angles) = 0;

    //! return the minimum and maximum value in degrees
    void prepare (const Archive*);

    //! draw the position angle as a function of pulse phase
    void draw (const Archive*);

    std::string get_flux_label (const Archive* data);

    //! Set the sub-integration to plot (where applicable)
    void set_subint (unsigned _isubint) { isubint = _isubint; }
    unsigned get_subint () const { return isubint; }
    
    //! Set the frequency channel to plot (where applicable)
    void set_chan (unsigned _ichan) { ichan = _ichan; }
    unsigned get_chan () const { return ichan; }

    //! will draw the error bar on each point
    void set_error_bars (bool bars) { error_bars = bars; }
    bool get_error_bars () const { return error_bars; }

    //! will draw only those points with linear > threshold * sigma
    void set_threshold (float t) { threshold = t; }
    float get_threshold () const { return threshold; }

    //! range of value in degrees
    void set_range (float t) { range = t; }
    float get_range () const { return range; }

  protected:

    //! Draw error bars
    bool error_bars;

    //! Noise threshold
    float threshold;

    //! Range of values in degrees
    float range;

    unsigned ichan;
    unsigned isubint;
  };

}

#endif
