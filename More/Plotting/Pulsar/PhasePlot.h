//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PhasePlot.h,v $
   $Revision: 1.17 $
   $Date: 2006/03/16 17:07:17 $
   $Author: straten $ */

#ifndef __Pulsar_PhasePlot_h
#define __Pulsar_PhasePlot_h

#include "Pulsar/SimplePlot.h"
#include "Pulsar/PhaseScale.h"

#include <vector>

namespace Pulsar {

  //! Simple plots with pulse phase along the x-axis
  class PhasePlot : public SimplePlot {

  public:

    //! Default constructor
    PhasePlot ();

    //! Get the text interface to the configuration attributes
    TextInterface::Class* get_interface ();

    //! Text interface to the PhasePlot class
    class Interface : public TextInterface::To<PhasePlot> {
    public:
      Interface (PhasePlot* = 0);
    };

    //! Get the default label for the x axis
    std::string get_xlabel (const Archive*);

    //! Get the default label for the y axis
    std::string get_ylabel (const Archive*);

    //! Set the y-range
    void set_yrange (float min, float max);

    //! Get the scale
    PhaseScale* get_scale ();

  };

}

#endif
