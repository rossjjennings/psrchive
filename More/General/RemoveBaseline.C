/***************************************************************************
 *
 *   Copyright (C) 2009 - 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/RemoveBaseline.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/MoreProfiles.h"
#include "Pulsar/Index.h"

#include "Pulsar/PhaseWeight.h"
#include "Pulsar/DisperseWeight.h"

#include <iostream>
using namespace std;

Pulsar::RemoveBaseline::RemoveBaseline ()
{
  profile_operation = new SubtractMean;
}

void Pulsar::RemoveBaseline::set_operation (Operation* op)
{
  profile_operation = op;
}

void Pulsar::RemoveBaseline::Total::transform (Archive* archive)
{
  const unsigned nsub = archive->get_nsubint();

  if (nsub == 0)
    return;

  Reference::To<PhaseWeight> baseline = archive->baseline();

  for (unsigned isub=0; isub < nsub; isub++)
    operate (archive->get_Integration(isub), baseline);
}

void Pulsar::RemoveBaseline::Total::operate (Integration* integration,
					     const PhaseWeight* baseline)
{
  DisperseWeight shift (integration);
  shift.set_weight (baseline);

  // the output of the PhaseWeight shifter
  PhaseWeight shifted_baseline;

  const unsigned nchan = integration->get_nchan();
  const unsigned npol = integration->get_npol();

  Index pscrunch;
  pscrunch.set_integrate (true);

  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    if (Profile::verbose)
      cerr << "Pulsar::RemoveBaseline::Total::operate ichan=" << ichan << endl;

    shift.get_weight (ichan, &shifted_baseline);

    // NormalizeBy operations apply a single scale factor to all polns
    shifted_baseline.set_Profile (get_Profile (integration,pscrunch,ichan));

    for (unsigned ipol=0; ipol<npol; ipol++)
    {
      if (Profile::verbose)
	cerr << "Pulsar::RemoveBaseline::Total::operate ipol=" << ipol << endl;

      Profile* profile = integration->get_Profile(ipol,ichan);

      profile_operation->operate (profile, &shifted_baseline);
    }
  }
}

void Pulsar::RemoveBaseline::Each::transform (Archive* archive)
{
  const unsigned nsub = archive->get_nsubint();
  const unsigned nchan = archive->get_nchan();
  const unsigned npol = archive->get_npol();

  Index pscrunch;
  pscrunch.set_integrate (true);

  for (unsigned isub=0; isub < nsub; isub++)
  {
    Integration* subint = archive->get_Integration (isub);
    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      Reference::To<const Profile> profile
	= get_Profile (subint, pscrunch, ichan);

      Reference::To<PhaseWeight> baseline = profile->baseline();

      for (unsigned ipol=0; ipol < npol; ipol++)
      {
	Profile* profile = subint->get_Profile(ipol, ichan);
	profile_operation->operate (profile, baseline);

	MoreProfiles* more = profile->get<MoreProfiles>();
	if (!more)
	  continue;

	unsigned nmore = more->get_size();
	for (unsigned imore=0; imore < nmore; imore++)
	{
	  profile = more->get_Profile (imore);
	  profile_operation->operate (profile, baseline);
	}
      } // for each poln
    } // for each chan
  } // for each subint
};

using Pulsar::RemoveBaseline;

RemoveBaseline::Operation*
RemoveBaseline::Operation::factory (const std::string& description)
{
  const string whitespace = " \t\n";

  string line = description;

  string key = stringtok (line, whitespace);
  string value = stringtok (line, whitespace);

  if (value == "by")
    value = stringtok (line, whitespace);

  if (key == "subtract" || key == "-=")
    {
      if (value == "avg" || value == "mean")
	return new SubtractMean;
      if (value == "med" || value == "median")
	return new SubtractMedian;
    }

  if (key == "normalize" || key == "divide" || key == "/=")
    {
     if (value == "avg" || value == "mean")
	return new NormalizeByMean;
      if (value == "med" || value == "median")
	return new NormalizeByMedian;
      if (value == "rms" || value == "stddev")
	return new NormalizeByStdDev;
      if (value == "mdm" || value == "mad" || value == "medabsdif")
	return new NormalizeByMedAbsDif;
    }
 
  throw Error (InvalidParam, "RemoveBaseline::Operation::factory",
	       "unrecognized description: '" + description + "'");
}


void RemoveBaseline::SubtractMean::operate (Profile* profile, 
					    const PhaseWeight* weight)
{
  weight->set_Profile (profile);

  double mean = weight->get_mean().val;

  if (Profile::verbose)
    cerr << "RemoveBaseline::SubtractMean::operate mean=" << mean << endl;

  profile->offset (-mean); 
}

void RemoveBaseline::SubtractMedian::operate (Profile* profile,
					      const PhaseWeight* weight)
{
  weight->set_Profile (profile);
  profile->offset (-weight->get_median()); 
}

void RemoveBaseline::NormalizeByMean::operate (Profile* profile,
					       const PhaseWeight* weight)
{
  profile->scale (1.0 / weight->get_mean().val); 
}

void RemoveBaseline::NormalizeByMedian::operate (Profile* profile,
						 const PhaseWeight* weight)
{
  profile->scale (1.0 / weight->get_median()); 
}

void RemoveBaseline::NormalizeByStdDev::operate (Profile* profile,
						 const PhaseWeight* weight)
{
  profile->scale (1.0 / weight->get_rms());
}

void RemoveBaseline::NormalizeByMedAbsDif::operate (Profile* profile,
						    const PhaseWeight* weight)
{
  profile->scale (1.0 / weight->get_median_difference());
}

