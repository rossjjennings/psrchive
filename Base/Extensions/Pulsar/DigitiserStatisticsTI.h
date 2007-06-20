//-*-C++-*-
/***************************************************************************
 *   Copyright (C) 2007 David smith
 *   Licensed under the Academic Free License version 2.1
 ***************************************************************************/



#ifndef __DigitiserStatisticsTI_h
#define __DigitiserStatisticsTI_h


#include <TextInterface.h>
#include <Pulsar/DigitiserStatistics.h>


// text interface to a DigistierStatics extension


namespace Pulsar
{
  class DigitiserStatisticsTI : public TextInterface::To< DigitiserStatistics >
  {
  public:
    DigitiserStatisticsTI();
    DigitiserStatisticsTI( DigitiserStatistics *c );
    void SetupMethods( void );

    virtual std::string get_interface_name() { return "DigitiserStatisticsTI"; }

    TextInterface::Class *clone();
  };
}

#endif

