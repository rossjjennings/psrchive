//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/EllAngPlot.h,v $
   $Revision: 1.4 $
   $Date: 2006/03/15 20:06:44 $
   $Author: straten $ */

#ifndef __Pulsar_EllAngPlot_h
#define __Pulsar_EllAngPlot_h

#include "Pulsar/AnglePlot.h"

namespace Pulsar {

  //! Plots the ellipticity angle of the polarized radiation
  class EllAngPlot : public AnglePlot {

  public:

    //! Return the default label for the y axis
    std::string get_ylabel (const Archive* data);
    
    //! Compute ellipticity angles and store in AnglePlot::angles
    void get_angles (const Archive*);

  };

}

#endif
