/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/





#include "Pulsar/DigitiserStatsPlot.h"
#include <Pulsar/DigitiserStatistics.h>
#include <iostream>




using namespace std;
using namespace Pulsar;



DigitiserStatsPlot::DigitiserStatsPlot()
{
}



void DigitiserStatsPlot::prepare( const Archive *data )
{
}



void DigitiserStatsPlot::draw( const Archive *data )
{
  Reference::To<Archive> clone = data->clone();

  Reference::To<DigitiserStatistics> ext = clone->get<DigitiserStatistics>();

  if( ext )
  {
    TextInterface::Parser *iface = ext->get_interface();
    if( iface )
    {
      cout << iface->process( "ndigr" ) << endl;
      cout << iface->process( "npar" ) << endl;
      cout << iface->process( "ncycsub" ) << endl;
      cout << iface->process( "diglev" ) << endl;
    }
  }
}



TextInterface::Parser *DigitiserStatsPlot::get_interface()
{
  return new Interface( this );
}



std::string DigitiserStatsPlot::get_xlabel( const Archive * data )
{
  return "phase";
}



std::string DigitiserStatsPlot::get_ylabel( const Archive *data )
{
  return "Digitiser Stats";
}







