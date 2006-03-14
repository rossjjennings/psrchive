//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PhaseScale.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/14 16:08:19 $
   $Author: straten $ */

#ifndef __Pulsar_PhaseScale_h
#define __Pulsar_PhaseScale_h

#include "Pulsar/PlotZoom.h"

namespace Pulsar {

  //! Plots the position angle of the linearly polarized radiation
  class PhaseScale : public PlotZoom {

  public:

    //! The units on the phase axis
    enum Scale { Turns, Degrees, Radians, Milliseconds };

    //! Default constructor
    PhaseScale ();

    //! Get a description of the scale
    std::string get_label ();

    //! Set the scale on the phase axis
    void set_scale (Scale s) { scale = s; }
    //! Get the scale on the phase axis
    Scale get_scale () const { return scale; }

    //! Set the world-normalized coordinates of the origin on the phase axis
    void set_origin_norm (float f) { origin_norm = f; }
    //! Get the world-normalized coordinates of the origin on the phase axis
    float get_origin_norm () const { return origin_norm; }

  protected:

    //! Scale on the phase axis
    Scale scale;

    //! Origin on the phase axis
    float origin_norm;


  };

  std::ostream& operator << (std::ostream& os, PhaseScale::Scale);
  std::istream& operator >> (std::istream& is, PhaseScale::Scale&);

}

#endif
