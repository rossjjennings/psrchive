//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_DynamicSpectrumPlot_h
#define __Pulsar_DynamicSpectrumPlot_h

#include "Pulsar/SimplePlot.h"
#include "Pulsar/TimeScale.h"
#include "Pulsar/FrequencyScale.h"
#include "Pulsar/Index.h"

#include "ColourMap.h"

namespace Pulsar {

  class Profile;

  //! Base class for image plots of something vs time and frequency
  class DynamicSpectrumPlot : public SimplePlot {

  public:

    //! Default constructor
    DynamicSpectrumPlot ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the DynamicSpectrumPlot class
    class Interface : public TextInterface::To<DynamicSpectrumPlot> {
    public:
      Interface (DynamicSpectrumPlot* = 0);
    };

    //! Derived classes must fill in the nsubint by nchan data array
    virtual void get_plot_array (const Archive *data, float *array) = 0;

    //! Draw in the current viewport
    void draw (const Archive*);

    //! Provide access to the colour map
    pgplot::ColourMap* get_colour_map () { return &colour_map; }

    std::pair<unsigned,unsigned> get_subint_range (const Archive* data);
    std::pair<unsigned,unsigned> get_chan_range (const Archive* data);

    //! srange is the subint range
    std::pair<int,int> get_srange() const;
    void set_srange( const std::pair<int,int> &range );
      
    //! Set the polarization to plot
    void set_pol (const Index& _ipol) { ipol = _ipol; }
    Index get_pol () const { return ipol; }

    //! Set a method
    void set_method (int s_method) { method = s_method; }
    int get_method() const { return method; }
    
    //! Return pointer to x scale
    TimeScale* get_x_scale () { return x_scale; }

    //! Return pointer to y scale
    FrequencyScale* get_y_scale () { return y_scale; }

  protected:

    pgplot::ColourMap colour_map;

    Index ipol;
    int method;

    bool zero_check;

    Reference::To<TimeScale> x_scale;
    Reference::To<FrequencyScale> y_scale;
  };

}

#endif
