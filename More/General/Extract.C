/***************************************************************************
 *
 *   Copyright (C) 2014 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Extract.h"
#include "Pulsar/Profile.h"

Pulsar::Extract::Extract (const Range& r)
{
  range = r;
  if (range.first >= range.second)
    throw Error (InvalidParam, "Pulsar::Extract ctor",
                 "range.first=%u >= range.second=%u", 
                 range.first, range.second); 
}

void Pulsar::Extract::transform (Profile* profile)
{
  std::vector<float> data;
  profile->get_amps (data);

  if (range.first >= data.size())
    throw Error (InvalidParam, "Pulsar::Extract::transform",
                 "range.first=%u >= nbin=%u", range.first, data.size());

  if (range.second >= data.size())
    throw Error (InvalidParam, "Pulsar::Extract::transform",
                 "range.second=%u >= nbin=%u", range.second, data.size());

  std::vector<float> subset ( data.begin()+range.first,
                              data.begin()+range.second );

  profile->set_amps (subset);
}

