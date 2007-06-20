/***************************************************************************
 *
 *   Copyright (C) {} 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/


#include "Pulsar/DigitiserCounts.h"
#include "Pulsar/DigitiserCountsTI.h"



using namespace Pulsar;






//Default constructor
DigitiserCounts::DigitiserCounts ()
    : Extension( "DigitiserCounts" ), nlev(0), npthist(0)
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

  return *this;
}

// Destructor
DigitiserCounts::~DigitiserCounts ()
{}


Reference::To< TextInterface::Class > DigitiserCounts::get_text_interface()
{
  return new DigitiserCountsTI( this );
}
