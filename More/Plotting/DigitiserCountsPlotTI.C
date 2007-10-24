/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "Pulsar/DigitiserCountsPlot.h"



using namespace Pulsar;



DigitiserCountsPlot::Interface::Interface( DigitiserCountsPlot *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  add( &DigitiserCountsPlot::get_fsub,
	&DigitiserCountsPlot::set_fsub,
	"fsub", "First sub integration to plot" );
  
  add( &DigitiserCountsPlot::get_lsub,
	&DigitiserCountsPlot::set_lsub,
	"lsub", "Last sub integration to plot" );
  
  add( &DigitiserCountsPlot::get_subint,
	&DigitiserCountsPlot::set_subint,
	"subint", "sub integration to plot" );
}


