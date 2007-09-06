//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/LinePhasePlot.h,v $
   $Revision: 1.1 $
   $Date: 2007/09/06 01:22:26 $
   $Author: nopeer $ */



#ifndef LINE_PHASE_PLOT_H_
#define LINE_PHASE_PLOT_H_



#include "Pulsar/PhaseVsPlot.h"



using namespace std;



namespace Pulsar
{
  class LinePhasePlot : public Pulsar::PhaseVsPlot
  {
  public:
    LinePhasePlot();
    ~LinePhasePlot();

    //! Derived classes must provide the number of rows
    virtual unsigned get_nrow (const Archive* arch);

    //! Derived classes must provide the Profile for the specified row
    virtual const Profile* get_Profile (const Archive* arch, unsigned row);
    
    virtual void prepare (const Archive* data);

  private:
    vector< Profile > data;
  };
}


#endif

