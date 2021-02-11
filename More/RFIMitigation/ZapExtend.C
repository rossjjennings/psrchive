/***************************************************************************
 *
 *   Copyright (C) 2019 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ZapExtend.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include <stdio.h>

using namespace std;

// Text interface to ZapExtend
TextInterface::Parser* Pulsar::ZapExtend::get_interface ()
{
  return new Interface (this);
}

Pulsar::ZapExtend::Interface::Interface (ZapExtend* instance)
{
  if (instance)
    set_instance (instance);

  add( &ZapExtend::get_time_cutoff,
       &ZapExtend::set_time_cutoff,
       "tcutoff", "Threshold (0->1) along time direction" );

  add( &ZapExtend::get_freq_cutoff,
       &ZapExtend::set_freq_cutoff,
       "fcutoff", "Threshold (0->1) along freq direction" );

  add( &ZapExtend::get_report,
       &ZapExtend::set_report,
       "report", "Print one-line report to stdout" );
}

// defined in More/General/standard_interface.C
std::string process (TextInterface::Parser* interface, const std::string& txt);

Pulsar::ZapExtend::ZapExtend ()
{
  time_cutoff = 0.8;
  freq_cutoff = 0.8;
  report = false;
}

void Pulsar::ZapExtend::transform (Archive* archive)
{
  const unsigned nchan = archive->get_nchan();
  const unsigned nsubint = archive->get_nsubint();
  std::vector<int> tcount;
  std::vector<int> fcount;

  tcount.resize(nchan, 0);
  fcount.resize(nsubint, 0);

  unsigned nonzapped = 0;
  
  for (unsigned isub=0; isub<nsubint; isub++)
  {
    Integration *subint = archive->get_Integration(isub);
    for (unsigned ichan=0; ichan<nchan; ichan++)
    {
      if (subint->get_weight(ichan) == 0.0)
      {
        tcount[ichan]++;
        fcount[isub]++;
      }
      else
	nonzapped ++;
    }
  }

  unsigned zapped = 0;
  
  for (unsigned isub=0; isub<nsubint; isub++)
  {
    Integration *subint = archive->get_Integration(isub);
    for (unsigned ichan=0; ichan<nchan; ichan++)
    {
      if (subint->get_weight(ichan) == 0.0)
	continue;
      
      if ( (tcount[ichan] > (time_cutoff*nsubint)) 
          || (fcount[isub] > (freq_cutoff*nchan)) )
      {
        subint->set_weight(ichan, 0.0);
	zapped ++;
      }
    }
  }

  if (report)
    cout << "extend tested=" << nonzapped << " masked=" << zapped
	 << " %=" << (zapped*100.0)/nonzapped << endl;
}
