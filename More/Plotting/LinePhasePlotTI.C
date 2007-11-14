


#include "Pulsar/LinePhasePlot.h"



using namespace Pulsar;



LinePhasePlot::Interface::Interface( LinePhasePlot *target )
{
  if( target )
    set_instance( target );

  import ( PhaseVsPlot::Interface() );

  add( &LinePhasePlot::get_isub, &LinePhasePlot::set_isub, "subint", "Sub integration to plot" );

  add( &LinePhasePlot::get_ipol, &LinePhasePlot::set_ipol, "ipol", "Polarization to plot" );
}
