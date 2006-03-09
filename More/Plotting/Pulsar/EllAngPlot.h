//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/EllAngPlot.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/09 17:59:30 $
   $Author: straten $ */

#ifndef __Pulsar_EllAngPlotter_h
#define __Pulsar_EllAngPlotter_h

#include "Pulsar/AnglePlotter.h"

namespace Pulsar {

  //! Plots the ellipticity angle of the polarized radiation
  class EllAngPlotter : public AnglePlotter {

  public:

    //! Default constructor
    EllAngPlotter ();

    //! Return the default label for the y axis
    std::string get_ylabel (const Archive* data);
    
    //! Derived types compute the angles to be plotted
    void get_angles (const Archive*, std::vector< Estimate<double> >&);

  };

}

#endif
