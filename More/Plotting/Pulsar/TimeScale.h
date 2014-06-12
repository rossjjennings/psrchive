//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2014 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/Pulsar/TimeScale.h

#ifndef __Pulsar_TimeScale_h
#define __Pulsar_TimeScale_h

#include "Pulsar/PlotScale.h"

namespace Pulsar {

  class Archive;

  //! Represents an axis with a time ordinate
  class TimeScale : public PlotScale {

  public:

    //! The units on the time axis
    enum Units { Time, HourAngle, MJD, Subint };

    //! Default constructor
    TimeScale ();

    // Text interface to the TimeScale class
    class Interface : public TextInterface::To<TimeScale>
    {
    public:
      Interface (TimeScale* = 0);
    };

    TextInterface::Parser* get_interface () { return new Interface(this); }

    //! Initialize internal attributes according to Archive data
    void init (const Archive*);

    //! Return the min and max as the sub-integration index
    void get_indeces (const Archive*, unsigned& min, unsigned& max) const;

    //! Get the ordinate values
    void get_ordinates (const Archive*, std::vector<float>& x_axis) const;

    //! Get a description of the units
    std::string get_label () const;

    //! Get the conversion factor from device to world normalized
    float get_scale (const Archive*) const;

    //! Set the units on the time axis
    void set_units (Units s) { units = s; }
    //! Get the units on the time axis
    Units get_units () const { return units; }

    //! Set the world-normalized coordinates of the origin on the time axis
    void set_origin_norm (float f) { origin_norm = f; }
    //! Get the world-normalized coordinates of the origin on the time axis
    float get_origin_norm () const { return origin_norm; }

  protected:

    //! Units on the time axis
    Units units;

    //! Origin on the time axis
    float origin_norm;

    //! Label on the time axis
    std::string label;

  };

  std::ostream& operator << (std::ostream& os, TimeScale::Units units);

  std::istream& operator >> (std::istream& is, TimeScale::Units& units);

}

#endif
