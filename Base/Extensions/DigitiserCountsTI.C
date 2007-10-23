/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/




#include "Pulsar/DigitiserCounts.h"



using namespace Pulsar;



DigitiserCounts::Interface::Interface( DigitiserCounts *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  add( &DigitiserCounts::get_dig_mode, "dig_mode", "Digitiser mode" );
  add( &DigitiserCounts::get_nlev, "nlev", "Number of digitiser levels" );
  add( &DigitiserCounts::get_npthist, "npthist", "Number of points in histogram (I)" );
  add( &DigitiserCounts::get_diglev, "diglev", "Digitiser level-setting mode (AUTO, FIX)" );
}



