/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/DigitiserCountsTI.h"


void Pulsar::DigitiserCountsTI::setup( void )
{
  add( &DigitiserCounts::get_dig_mode, "dig_mode", "Digitiser mode" );
  add( &DigitiserCounts::get_nlev, "nlev", "Number of digitiser levels" );
  add( &DigitiserCounts::get_npthist, "npthist", "Number of points in histogram (I)" );
  add( &DigitiserCounts::get_diglev, "diglev", "Digitiser level-setting mode (AUTO, FIX)" );
}

TextInterface::Parser *Pulsar::DigitiserCountsTI::clone()
{
  if( instance )
    return new DigitiserCountsTI( instance );
  else
    return new DigitiserCountsTI();
}

