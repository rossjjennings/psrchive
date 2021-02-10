/***************************************************************************
 *
 *   Copyright (C) 2018 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/TimeFrequencySmooth.h"
#include "Pulsar/DoubleMedian.h"
#include "Pulsar/TwoDeeMedian.h"

#include "interface_factory.h"
#include "interface_stream.h"

#include <assert.h>

Pulsar::TimeFrequencySmooth::TimeFrequencySmooth ()
{
}

void Pulsar::TimeFrequencySmooth::check_dimensions(std::vector<float> &smoothed,
    std::vector<float> &raw, std::vector<float> &weight,
    std::vector<float> &freqs, std::vector<float> &times)
{
  // Determine dimensions
  nsub = times.size();
  nchan = freqs.size() / nsub;
  npol = raw.size() / nchan / nsub;

  // Check for dimension problems
  if (freqs.size() % nsub) 
    throw Error (InvalidParam, "Pulsar::TimeFrequencySmooth::check_dimensions",
        "freq array bad size (size=%d nsub=%d)", freqs.size(), nsub);

  if (weight.size() != nsub*nchan)
    throw Error (InvalidParam, "Pulsar::TimeFrequencySmooth::check_dimensions",
        "weight array bad size");

  if (raw.size() % (nsub*nchan))
    throw Error (InvalidParam, "Pulsar::TimeFrequencySmooth::check_dimensions",
        "raw array bad size");

  smoothed.resize(nsub*nchan*npol);
}

static std::vector< Pulsar::TimeFrequencySmooth* >* instances = NULL;

void Pulsar::TimeFrequencySmooth::build ()
{
  if (instances != NULL)
    return;

  instances = new std::vector< TimeFrequencySmooth* >;
 
  instances->push_back( new DoubleMedian );
  instances->push_back( new TwoDeeMedian );
}

const std::vector<Pulsar::TimeFrequencySmooth*>& 
Pulsar::TimeFrequencySmooth::children ()
{
  if (instances == NULL)
    build ();

  assert (instances != NULL);

  return *instances;
}

Pulsar::TimeFrequencySmooth*
Pulsar::TimeFrequencySmooth::factory (const std::string& name)
{
  return TextInterface::factory<TimeFrequencySmooth> (children(), name);
}

namespace Pulsar
{
  std::ostream& operator<< (std::ostream& ostr,
                            TimeFrequencySmooth* stat)
  {
    return interface_insertion (ostr, stat);
  }

  std::istream& operator>> (std::istream& istr,
                            TimeFrequencySmooth* &stat)
  {
    return interface_extraction (istr, stat);
  }
}


