//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PosAngPlot.h,v $
   $Revision: 1.6 $
   $Date: 2006/03/11 22:14:46 $
   $Author: straten $ */

#ifndef __Pulsar_PosAngPlot_h
#define __Pulsar_PosAngPlot_h

#include "Pulsar/AnglePlot.h"

namespace Pulsar {

  //! Plots the position angle of the linearly polarized radiation
  class PosAngPlot : public AnglePlot {

  public:

    //! Return the default label for the y axis
    std::string get_ylabel (const Archive* data);
    
    //! Derived types compute the angles to be plotted
    void get_angles (const Archive*);

  };

}

#endif
