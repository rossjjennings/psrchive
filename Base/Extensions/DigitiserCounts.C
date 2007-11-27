/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/


#include "Pulsar/DigitiserCounts.h"




using namespace Pulsar;






//Default constructor
DigitiserCounts::DigitiserCounts ()
    : Extension( "DigitiserCounts" ), nlev(0), npthist(0), ndigr(0), dyn_levt(0)
{}

// Copy constructor
DigitiserCounts::DigitiserCounts (const Pulsar::DigitiserCounts& extension)
    : Extension( "DigitiserCounts" )
{
  operator = (extension);
}

// Operator =
const DigitiserCounts& DigitiserCounts::operator= (const DigitiserCounts& extension)
{
  dig_mode = extension.dig_mode;
  nlev = extension.nlev;
  npthist = extension.npthist;
  diglev = extension.diglev;
  ndigr = extension.ndigr;
  rows = extension.rows;

  return *this;
}

// Destructor
DigitiserCounts::~DigitiserCounts ()
{}


TextInterface::Parser* DigitiserCounts::get_interface()
{
  return new Interface( this );
}




/////////////////////// row

DigitiserCounts::row::row()
{
}



DigitiserCounts::row::~row()
{
}


