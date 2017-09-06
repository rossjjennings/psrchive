/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/psrgui/Pulsar/UsingQTDRIV.h

#ifndef __Pulsar_UsingQTDRIV_h
#define __Pulsar_UsingQTDRIV_h

#include "Pulsar/PlotWindow.h"
#include "qpgplot.h"

namespace Pulsar {
  
  //! Manages a window in which to Plot an Archive
  class UsingQTDRIV : public PlotWindow, public QPgplot
  {
  public:
  
    UsingQTDRIV( QWidget *parent=0, const char *name=0 );
    
    //! Implement QtPglot::plotter
    void plotter ();

    //! Re-implement PlotWindow::plot_data
    void plot_data ();
  };

}

#endif

