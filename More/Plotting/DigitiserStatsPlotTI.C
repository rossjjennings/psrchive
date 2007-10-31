




#include "Pulsar/DigitiserStatsPlot.h"



using namespace Pulsar;




DigitiserStatsPlot::Interface::Interface( DigitiserStatsPlot *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  add( &DigitiserStatsPlot::get_subint,
         &DigitiserStatsPlot::set_subint,
         "subint", "Sub integration" );

  add( &DigitiserStatsPlot::get_fsub,
         &DigitiserStatsPlot::set_fsub,
         "fsub", "First Subintegration" );

  add( &DigitiserStatsPlot::get_lsub,
         &DigitiserStatsPlot::set_lsub,
         "lsub", "Last Subintegration" );
}


