//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PosAngPlot.h,v $
   $Revision: 1.7 $
   $Date: 2006/03/15 20:06:44 $
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
    
    //! Compute position angles and store in AnglePlot::angles
    void get_angles (const Archive*);

  };

}

#endif
