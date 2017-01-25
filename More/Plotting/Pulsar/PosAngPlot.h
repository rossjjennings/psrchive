//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/Pulsar/PosAngPlot.h

#ifndef __Pulsar_PosAngPlot_h
#define __Pulsar_PosAngPlot_h

#include "Pulsar/AnglePlot.h"

namespace Pulsar {

  //! Plots the position angle of the linearly polarized radiation
  class PosAngPlot : public AnglePlot {

  public:

    //! Default constructor
    PosAngPlot ();

    //! Return the default label for the y axis
    std::string get_ylabel (const Archive* data);
    
    //! Compute position angles and store in AnglePlot::angles
    void get_angles (const Archive*);

  };

}

#endif
