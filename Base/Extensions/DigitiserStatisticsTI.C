//-*-C++-*-
/***************************************************************************
 *   Copyright (C) 2007 David smith
 *   Licensed under the Academic Free License version 2.1
 ***************************************************************************/



#include <Pulsar/DigitiserStatistics.h>



using namespace Pulsar;



DigitiserStatistics::Interface::Interface( DigitiserStatistics *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  add( &DigitiserStatistics::get_ndigr, "ndigr", "Number of digitised channels (I)" );
  add( &DigitiserStatistics::get_npar, "npar", "Number of digitiser parameters" );
  add( &DigitiserStatistics::get_ncycsub, "ncycsub", "Number of correlator cycles per subint" );
  add( &DigitiserStatistics::get_diglev, "diglev", "Digitiser level-setting mode (AUTO, FIX)" );
}






