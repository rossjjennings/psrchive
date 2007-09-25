//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/AnglePlot.h,v $
   $Revision: 1.13 $
   $Date: 2007/09/25 14:12:06 $
   $Author: straten $ */

#ifndef __Pulsar_AnglePlot_h
#define __Pulsar_AnglePlot_h

#include "Pulsar/PhasePlot.h"
#include "Pulsar/PlotIndex.h"

#include "Estimate.h"

namespace Pulsar {

  //! Plots an angle and its error as a function of pulse phase
  class AnglePlot : public PhasePlot {

  public:

    enum Mark { ErrorTick=0x01, ErrorBar=0x02, Dot=0x04 };

    //! Default constructor
    AnglePlot ();

    //! Get the text interface to the configuration attributes
    TextInterface::Class* get_interface ();

    // Text interface to the AnglePlot class
    class Interface : public TextInterface::To<AnglePlot> {
    public:
      Interface (AnglePlot* = 0);
    };

    //! Derived types compute the angles to be plotted
    virtual void get_angles (const Archive*) = 0;

    //! return the minimum and maximum value in degrees
    void prepare (const Archive*);

    //! draw the angle as a function of pulse phase
    void draw (const Archive*);

    std::string get_flux_label (const Archive* data);

    //! Set the sub-integration to plot (where applicable)
    void set_subint (const PlotIndex& _isubint) { isubint = _isubint; }
    PlotIndex get_subint () const { return isubint; }
    
    //! Set the frequency channel to plot (where applicable)
    void set_chan (const PlotIndex& _ichan) { ichan = _ichan; }
    PlotIndex get_chan () const { return ichan; }

    //! the mark to be used
    void set_marker (const std::string&);
    std::string get_marker () const;

    //! will draw only those points with linear > threshold * sigma
    void set_threshold (float t) { threshold = t; }
    float get_threshold () const { return threshold; }

    //! span of value in degrees
    void set_span (float t) { span = t; }
    float get_span () const { return span; }

  protected:

    //! Marker used to plot each point
    int marker;

    //! Noise threshold
    float threshold;

    //! Span of values in degrees
    float span;

    //! The angles
    std::vector< Estimate<double> > angles;

    PlotIndex ichan;
    PlotIndex isubint;
  };

}

#endif
