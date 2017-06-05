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
}

void Pulsar::InterQuartileRange::transform (Archive* archive)
{
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
  unsigned valid = 0;
  
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
	values[valid] = var;
      }
      valid ++;
    }
  }
  
  std::vector<float> val (values.begin(), values.begin()+ valid);
    
  unsigned iq1 = valid/4;
  unsigned iq3 = (valid*3)/4;
    
  std::nth_element (val.begin(), val.begin()+iq1, val.begin()+valid);
  double Q1 = values[ iq1 ];
  
  std::nth_element (val.begin()+iq1, val.begin()+iq3, val.begin()+valid);
  double Q3 = values[ iq3 ];
  
  double IQR = Q3 - Q1;

  unsigned revisit = 0;
  
  for (unsigned isubint=0; isubint < nsubint; isubint++)
  {
    Integration* subint = archive->get_Integration( isubint );
    
    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      if (subint->get_weight(ichan) == 0)
	continue;

      if (values[revisit] < Q1 - cutoff_threshold * IQR)
	subint->set_weight(ichan, 0);

      if (values[revisit] > Q3 + cutoff_threshold * IQR)
	subint->set_weight(ichan, 0);

      revisit ++;
    }
  }

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
