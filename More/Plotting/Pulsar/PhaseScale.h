//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PhaseScale.h,v $
   $Revision: 1.8 $
   $Date: 2006/03/16 17:07:17 $
   $Author: straten $ */

#ifndef __Pulsar_PhaseScale_h
#define __Pulsar_PhaseScale_h

#include "Pulsar/PlotScale.h"

namespace Pulsar {

  class Archive;

  //! Reperesents an axis with pulse phase ordinate
  class PhaseScale : public PlotScale {

  public:

    //! The units on the phase axis
    enum Units { Turns, Degrees, Radians, Milliseconds };

    //! Default constructor
    PhaseScale ();

    //! Text interface to the PhaseScale class
    class Interface : public TextInterface::To<PhaseScale> {
    public:
      Interface (PhaseScale* = 0);
    };

    //! Disable set_minmax
    void set_minmax (float min, float max);

    //! Return the min and max in the current units
    void get_range (const Archive*, float& min, float& max) const;

    //! Return the min and max as the phase bin index
    void get_range (const Archive*, unsigned& min, unsigned& max) const;

    //! Get the ordinate values
    void get_ordinates (const Archive* data, std::vector<float>& axis) const;

    //! Get a description of the units
    std::string get_label () const;

    //! Get the conversion factor from turns to units
    float get_scale (const Archive*) const;

    //! Set the units on the phase axis
    void set_units (Units s) { units = s; }
    //! Get the units on the phase axis
    Units get_units () const { return units; }

    //! Set the world-normalized coordinates of the origin on the phase axis
    void set_origin_norm (float f) { origin_norm = f; }
    //! Get the world-normalized coordinates of the origin on the phase axis
    float get_origin_norm () const { return origin_norm; }

  protected:

    //! Units on the phase axis
    Units units;

    //! Origin on the phase axis
    float origin_norm;

  };

  std::ostream& operator << (std::ostream& os, PhaseScale::Units);
  std::istream& operator >> (std::istream& is, PhaseScale::Units&);

}

#endif
