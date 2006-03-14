//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/MultiPhase.h,v $
   $Revision: 1.8 $
   $Date: 2006/03/14 16:08:57 $
   $Author: straten $ */

#ifndef __Pulsar_MultiPhase_h
#define __Pulsar_MultiPhase_h

#include "Pulsar/MultiPlot.h"
#include "Pulsar/PhaseScale.h"

namespace Pulsar {

  //! Plots multiple viewports with pulse phase along the shared x-axis
  class MultiPhase : public MultiPlot {

  public:

    //! Default constructor
    MultiPhase ();

    //! Manage a plot
    void manage (const std::string& name, Plot* plot);

    //! Get the scale
    PhaseScale* get_scale ();

  };

}

#endif
