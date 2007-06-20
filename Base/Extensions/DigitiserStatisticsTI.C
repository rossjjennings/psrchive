//-*-C++-*-
/***************************************************************************
 *   Copyright (C) 2007 David smith
 *   Licensed under the Academic Free License version 2.1
 ***************************************************************************/



#include <Pulsar/DigitiserStatisticsTI.h>



using namespace Pulsar;

DigitiserStatisticsTI::DigitiserStatisticsTI()
{
  SetupMethods();
}


DigitiserStatisticsTI::DigitiserStatisticsTI( DigitiserStatistics *c )
{
  SetupMethods();
  set_instance( c );
}

void DigitiserStatisticsTI::SetupMethods( void )
{
  add( &DigitiserStatistics::get_ndigr, "ndigr", "Number of digitised channels (I)" );
  add( &DigitiserStatistics::get_npar, "npar", "Number of digitiser parameters" );
  add( &DigitiserStatistics::get_ncycsub, "ncycsub", "Number of correlator cycles per subint" );
  add( &DigitiserStatistics::get_diglev, "diglev", "Digitiser level-setting mode (AUTO, FIX)" );
}


TextInterface::Class *Pulsar::DigitiserStatisticsTI::clone()
{
  if( instance )
    return new DigitiserStatisticsTI( instance );
  else
    return new DigitiserStatisticsTI();
}




