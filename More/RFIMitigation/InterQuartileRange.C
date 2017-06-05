/***************************************************************************
 *
 *   Copyright (C) 2017 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/InterQuartileRange.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/ProfileStats.h"
#include "Pulsar/Index.h"

#include <algorithm>
#include <assert.h>

using namespace std;

// defined in More/General/standard_interface.C
std::string process (TextInterface::Parser* interface, const std::string& txt);

Pulsar::InterQuartileRange::InterQuartileRange ()
{
  cutoff_threshold = 1.5;
  max_iterations = 15;
}

void Pulsar::InterQuartileRange::transform (Archive* archive)
{
  unsigned tot_valid = 0;
  unsigned tot_high = 0;
  unsigned tot_low = 0;
  unsigned iter = 0;
  
  while (iter < max_iterations)
  {
    once (archive);

    if (iter == 0)
      tot_valid = valid;

    tot_high += too_high;
    tot_low += too_low;
    iter ++;
    
    if (too_high + too_low == 0)
      break;
  }

  cerr << "Pulsar::InterQuartileRange::transform tested=" << tot_valid
       << " iter=" << iter << " high=" << tot_high << " low=" << tot_low
       << " %=" << (tot_high+tot_low)*100.0/tot_valid << endl;
}

void Pulsar::InterQuartileRange::once (Archive* archive)
{
  too_high = 0;
  too_low = 0; 
  valid = 0;

  Reference::To<ProfileStats> stats;
  Reference::To<TextInterface::Parser> parser;

  if (!expression.empty())
  {
    stats = new ProfileStats;
    parser = stats->get_interface ();
  }

  unsigned nchan = archive->get_nchan();
  unsigned nsubint = archive->get_nsubint();
  
  std::vector<float> values (nchan * nsubint);
  
  for (unsigned isubint=0; isubint < nsubint; isubint++)
  {
    Integration* subint = archive->get_Integration( isubint );
    
    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      if (subint->get_weight(ichan) == 0)
	continue;

      Index pol (0, true); // integrate over polarizations
      Reference::To<const Profile> profile
	= Pulsar::get_Profile (subint, pol, ichan);
	
      if (stats)
      {
	stats->set_Profile (profile);
	string value = process( parser, expression );
	values[valid] = fromstring<float>( value );
      }
      else
      {
	double mean = 0;
	double var = 0;
	profile->stats (&mean, &var);
	values[valid] = sqrt(var)/mean; // i.e. the modulation index
      }
      valid ++;
    }
  }

#ifdef _DEBUG
  cerr << "nchan=" << nchan << " nsub=" << nsubint << " nchan*nsub=" << nchan*nsubint << " valid=" << valid << endl;
#endif

  std::vector<float> val (values.begin(), values.begin()+ valid);
    
  unsigned iq1 = valid/4;
  unsigned iq3 = (valid*3)/4;

#ifdef _DEBUG
  cerr << "iQ1=" << iq1 << " iQ3=" << iq3 << endl;
#endif

  std::sort (val.begin(), val.begin()+valid);
  double Q1 = val[ iq1 ];
  double Q3 = val[ iq3 ];

  double IQR = Q3 - Q1;

#ifdef _DEBUG
  cerr << "Q1=" << Q1 << " Q3=" << Q3 << " IQR=" << IQR << endl;
#endif

  unsigned revisit = 0;
  for (unsigned isubint=0; isubint < nsubint; isubint++)
  {
    Integration* subint = archive->get_Integration( isubint );
    
    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      if (subint->get_weight(ichan) == 0)
	continue;

      if (values[revisit] < Q1 - cutoff_threshold * IQR)
      {
	subint->set_weight(ichan, 0);
        too_low ++;
      }

      if (values[revisit] > Q3 + cutoff_threshold * IQR)
      {
	subint->set_weight(ichan, 0);
        too_high ++;
      }

      revisit ++;
    }
  }

#ifdef _DEBUG
  cerr << "too high=" << too_high << " too low=" << too_low << endl;
#endif

  assert (revisit == valid);
}


//! Get the text interface to the configuration attributes
TextInterface::Parser* Pulsar::InterQuartileRange::get_interface ()
{
  return new Interface (this);
}

Pulsar::InterQuartileRange::Interface::Interface (InterQuartileRange* instance)
{
  if (instance)
    set_instance (instance);

  add( &InterQuartileRange::get_expression,
       &InterQuartileRange::set_expression,
       "exp", "Statistical expression" );

  add( &InterQuartileRange::get_cutoff_threshold,
       &InterQuartileRange::set_cutoff_threshold,
       "cutoff", "Outlier threshold: Q1-cutoff*IQR - Q3+cutoff*IQR" );
}
