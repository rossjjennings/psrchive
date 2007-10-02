//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/LinePhasePlot.h,v $
   $Revision: 1.3 $
   $Date: 2007/10/02 05:08:15 $
   $Author: straten $ */



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

    unsigned get_nrow (const Archive* arch);

    const Profile* get_Profile (const Archive* arch, unsigned row);

    void prepare (const Archive* data);

  class Interface : public TextInterface::To<LinePhasePlot>
    {
    public:
      Interface( LinePhasePlot *target = NULL );
    };

    TextInterface::Parser *get_interface();

    int get_isub( void ) const { return isub; }
    void set_isub( int s_isub ) { isub = s_isub; }

    int get_ipol( void ) const { return ipol; }
    void set_ipol( int s_ipol ) { ipol = s_ipol; }

  private:
    vector< Reference::To<Profile> > data;
    int ipol;
    int ichan;
    int isub;
  };
}


#endif

