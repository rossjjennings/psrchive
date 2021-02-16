/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/TimeFrequencyMask.h"

#include "Pulsar/SetThresholds.h"
#include "Pulsar/SumThreshold.h"
#include "Pulsar/InterQuartileRange.h"

#include "interface_factory.h"
#include "interface_stream.h"

#include <assert.h>

static std::vector< Pulsar::TimeFrequencyMask* >* instances = NULL;

Pulsar::TimeFrequencyMask::TimeFrequencyMask ()
{
  threshold = 3.0;
}

void Pulsar::TimeFrequencyMask::build ()
{
  if (instances != NULL)
    return;

  instances = new std::vector< TimeFrequencyMask* >;
 
  instances->push_back( new SetThresholds );
  instances->push_back( new SumThreshold );
  instances->push_back( new InterQuartileRange );
}

const std::vector<Pulsar::TimeFrequencyMask*>& 
Pulsar::TimeFrequencyMask::children ()
{
  if (instances == NULL)
    build ();

  assert (instances != NULL);

  return *instances;
}

Pulsar::TimeFrequencyMask*
Pulsar::TimeFrequencyMask::factory (const std::string& name)
{
  return TextInterface::factory<TimeFrequencyMask> (children(), name);
}

namespace Pulsar
{
  std::ostream& operator<< (std::ostream& ostr,
                            TimeFrequencyMask* stat)
  {
    return interface_insertion (ostr, stat);
  }

  std::istream& operator>> (std::istream& istr,
                            TimeFrequencyMask* &stat)
  {
    return interface_extraction (istr, stat);
  }
}

