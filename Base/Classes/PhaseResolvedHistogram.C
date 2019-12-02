/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseResolvedHistogram.h"

//! Construct with a name
Pulsar::PhaseResolvedHistogram::PhaseResolvedHistogram (const char* name)
  : MoreProfiles (name)
{
  min = max = 0.0;
}

Pulsar::PhaseResolvedHistogram* Pulsar::PhaseResolvedHistogram::clone () const
{
  return new PhaseResolvedHistogram (*this);
}

//! average information from another MoreProfiles
void Pulsar::PhaseResolvedHistogram::average (const MoreProfiles* more)
{
  if (!dynamic_cast<const PhaseResolvedHistogram*> (more))
    return;

  MoreProfiles::average (more);
}

//! Set the minimum and maximum values spanned by the histogram
void Pulsar::PhaseResolvedHistogram::set_range (double _min, double _max)
{
  min = _min;
  max = _max;
}

//! get the Profile that corresponds to the specified value
Pulsar::Profile* Pulsar::PhaseResolvedHistogram::at (double value)
{
  if (value < min || value > max)
    throw Error (InvalidParam, "Pulsar::PhaseResolvedHistogram::at",
		 "value=%lf out of range (%lf -> %lf)", value, min, max);

  unsigned max_index = profile.size();
  unsigned index = max_index * (value - min) / (max - min);

  return get_Profile( index%max_index );
}
