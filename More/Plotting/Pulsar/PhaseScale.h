//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PhaseScale.h,v $
   $Revision: 1.3 $
   $Date: 2006/03/14 16:50:41 $
   $Author: straten $ */

#ifndef __Pulsar_PhaseScale_h
#define __Pulsar_PhaseScale_h

#include "Pulsar/PlotScale.h"

namespace Pulsar {

  //! Plots the position angle of the linearly polarized radiation
  class PhaseScale : public PlotScale {

  public:

    //! The units on the phase axis
    enum Units { Turns, Degrees, Radians, Milliseconds };

    //! Default constructor
    PhaseScale ();

    //! Get a description of the units
    std::string get_label ();

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
