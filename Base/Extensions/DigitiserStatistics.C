//-*-C++-*-

#include <string.h>

#include "Pulsar/DigitiserStatistics.h"

// //////////////////////////////////////////////////
// DigitiserStatistics methods
//

Pulsar::DigitiserStatistics::DigitiserStatistics () 
  : Extension ("DigitiserStatistics")
{ 
  init(); 
}
 
Pulsar::DigitiserStatistics::DigitiserStatistics (const DigitiserStatistics& extension)
  : Extension ("DigitiserStatistics")
{
  rows = extension.rows;
}

const Pulsar::DigitiserStatistics& 
Pulsar::DigitiserStatistics::operator= (const DigitiserStatistics& extension)
{
  rows = extension.rows;
  return *this;
}

Pulsar::DigitiserStatistics::~DigitiserStatistics ()
{
}

void Pulsar::DigitiserStatistics::init ()
{
  rows.resize(0);
}

Pulsar::DigitiserStatistics::row& Pulsar::DigitiserStatistics::get_row (unsigned i)
{
  return rows[i];
}

Pulsar::DigitiserStatistics::row& Pulsar::DigitiserStatistics::get_last ()
{
  return rows.back();
}

void Pulsar::DigitiserStatistics::push_blank_row ()
{
  rows.push_back(row());
  rows.back().index = rows.size();
}

// Row methods

Pulsar::DigitiserStatistics::row::~row ()
{
}

void Pulsar::DigitiserStatistics::row::init ()
{
  dig_mode = "unknown";
  diglev   = "unknown";

  ndigr   = 0;
  nlev    = 0;
  ncycsub = 0;

  data.resize(0);
}
