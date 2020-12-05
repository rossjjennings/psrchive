//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/Pulsar/PhaseScale.h

#ifndef __Pulsar_PhaseScale_h
#define __Pulsar_PhaseScale_h

#include "Pulsar/PlotScale.h"
#include "PhaseRange.h"

namespace Pulsar {

  class Archive;

  //! Reperesents an axis with pulse phase ordinate
  class PhaseScale : public PlotScale {

  public:

    //! Default constructor
    PhaseScale ();

    // Text interface to the PhaseScale class
    class Interface : public TextInterface::To<PhaseScale> {
    public:
      Interface (PhaseScale* = 0);
    };

    //! Initialize internal attributes according to Archive data
    void init (const Archive*);

    //! Return the min and max as the phase bin index
    void get_indeces (const Archive*, unsigned& min, unsigned& max) const;

    //! Get the ordinate values
    void get_ordinates (const Archive* data, std::vector<float>& axis) const;

    //! Get a description of the units
    std::string get_label () const;

    //! Get the conversion factor from turns to units
    float get_scale (const Archive*) const;
    float get_scale (const Archive*, Phase::Unit) const;

    //! Set the units on the phase axis
    void set_units (Phase::Unit s) { units = s; }
    //! Get the units on the phase axis
    Phase::Unit get_units () const { return units; }

    //! Set the world-normalized coordinates of the origin on the phase axis
    void set_origin_norm (float f) { origin_norm = f; }
    //! Get the world-normalized coordinates of the origin on the phase axis
    float get_origin_norm () const { return origin_norm; }

    void set_origin (float f) { origin = f; origin_units = units; }
    float get_origin () const { return origin; }

  protected:

    //! Units on the phase axis
    Phase::Unit units;

    //! Units at the time set_origin is called
    Phase::Unit origin_units;

    //! Origin on the phase axis in world-normalized coordinates
    float origin_norm;

    //! Origin on the phase axis
    float origin;
  };
}

#endif
