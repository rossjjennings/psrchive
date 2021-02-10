/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/TimeFrequencyMask.h"
#include "Pulsar/SumThreshold.h"

#include "interface_factory.h"
#include "interface_stream.h"

static std::vector< Pulsar::TimeFrequencyMask* >* instances = NULL;

void Pulsar::TimeFrequencyMask::build ()
{
  if (instances != NULL)
    return;

  instances = new std::vector< TimeFrequencyMask* >;
 
  instances->push_back( new SumThreshold );
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

