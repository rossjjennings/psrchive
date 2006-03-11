//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/EllAngPlot.h,v $
   $Revision: 1.3 $
   $Date: 2006/03/11 22:14:46 $
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
    
    //! Derived types compute the angles to be plotted
    void get_angles (const Archive*);

  };

}

#endif
