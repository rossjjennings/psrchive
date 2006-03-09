//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PosAngPlot.h,v $
   $Revision: 1.4 $
   $Date: 2006/03/09 17:59:26 $
   $Author: straten $ */

#ifndef __Pulsar_PosAngPlotter_h
#define __Pulsar_PosAngPlotter_h

#include "Pulsar/AnglePlotter.h"

namespace Pulsar {

  //! Plots the position angle of the linearly polarized radiation
  class PosAngPlotter : public AnglePlotter {

  public:

    //! Return the default label for the y axis
    std::string get_ylabel (const Archive* data);
    
    //! Derived types compute the angles to be plotted
    void get_angles (const Archive*, std::vector< Estimate<double> >&);

  };

}

#endif
