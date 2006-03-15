//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PhasePlot.h,v $
   $Revision: 1.15 $
   $Date: 2006/03/15 11:38:35 $
   $Author: straten $ */

#ifndef __Pulsar_PhasePlot_h
#define __Pulsar_PhasePlot_h

#include "Pulsar/SimplePlot.h"
#include "Pulsar/PhaseScale.h"

#include <vector>

namespace Pulsar {

  //! Simple plots with pulse phase along the x-axis
  /*! Children of the PhasePlot always:
    <UL>
    <LI> plot in the currently open viewport
    <LI> use the currently set line width
    <LI> use the currently set character height
    </UL>
    That is, children of the PhasePlot class do not call
    cpgsvp, cpgslw, or cpgsch.  Children of the PhasePlot class may:
    <UL>
    <LI> change the colour used within the plot frame
    <LI> change the line style within the plot frame
    </UL>
  */
  class PhasePlot : public SimplePlot {

  public:

    //! Default constructor
    PhasePlot ();

    //! Get the text interface to the configuration attributes
    TextInterface::Class* get_interface ();

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
