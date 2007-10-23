//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

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
  ndigr = extension.ndigr;
  npar = extension.npar;
  ncycsub = extension.ncycsub;
  diglev = extension.diglev;
  rows = extension.rows;
}

const Pulsar::DigitiserStatistics& 
Pulsar::DigitiserStatistics::operator= (const DigitiserStatistics& extension)
{
  ndigr = extension.ndigr;
  npar = extension.npar;
  ncycsub = extension.ncycsub;
  diglev = extension.diglev;
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



//! Return a text interfaces that can be used to access this instance
TextInterface::Parser* Pulsar::DigitiserStatistics::get_interface()
{
	return new Interface( this );
}
