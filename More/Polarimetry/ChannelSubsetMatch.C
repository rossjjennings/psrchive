/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ChannelSubsetMatch.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Database.h"
#include "Pulsar/PolnCalibratorExtension.h"

#include "strutil.h"

using namespace std;
using namespace Pulsar;

ChannelSubsetMatch::ChannelSubsetMatch()
{
  freq_tol = 1e-7;
  bw_tol   = 1e-3;

  reason="";
}

ChannelSubsetMatch::~ChannelSubsetMatch()
{
}

double get_channel_frequency (const Archive* archive, unsigned ichan)
{
  // Use a CalExtension if it is present (for stored calibration solutions)
  // otherwise use subint 0
  const CalibratorExtension* ext = 
    archive->get<CalibratorExtension>();
  if (ext)
    return ext->get_centre_frequency (ichan);
  else
    return archive->get_Integration(0)->get_centre_frequency (ichan);
}

bool ChannelSubsetMatch::match (const Archive* super,
    const Archive* sub)
{
  reason="";

  // If n_sub > n_super, we're done
  if (sub->get_nchan() > super->get_nchan()) {
    reason = stringprintf("Subset n_chan (%d) > n_chan (%d)", 
        sub->get_nchan(), super->get_nchan());
    return false;
  }

  const double sub_chbw = sub->get_bandwidth() / (double)sub->get_nchan();
  const double super_chbw = super->get_bandwidth() / (double)super->get_nchan();

  // If channel BWs don't match, return false for now.
  if (fabs((sub_chbw-super_chbw)/super_chbw) > bw_tol) {
    reason = stringprintf("Subset chan_bw (%f) != chan_bw (%f)", 
        sub_chbw, super_chbw);
    return false;
  }

  // Set fractional freq tolerance to either 1% of channel BW
  // or the default, whichever is larger.
  double freq_tol_match = fabs(1e-2*sub_chbw/get_channel_frequency(sub,0));
  if (freq_tol > freq_tol_match) { freq_tol_match = freq_tol; }

  // Loop over "sub" chans, make sure they all exist in super
  unsigned n_matched=0;
  for (unsigned i=0; i<sub->get_nchan(); i++) {
    double sub_freq = sub->get_Integration(0)->get_centre_frequency(i);
    for (unsigned j=0; j<super->get_nchan(); j++) {
      double super_freq = super->get_Integration(0)->get_centre_frequency(j);
      if (fabs((sub_freq-super_freq)/super_freq) < freq_tol_match) {
        n_matched++;
        break;
      }
    }
  }

  if (n_matched==sub->get_nchan())
    return true;
  else {
    reason = stringprintf("Only matched %u/%u channels (archives)", 
        n_matched, sub->get_nchan());
    return false;
  }
}

bool ChannelSubsetMatch::match (const Database::StaticEntry* super,
				const Database::StaticEntry* sub)
{
  reason="";

  // If n_sub > n_super, we can exit right away
  if (sub->nchan > super->nchan) {
    reason = stringprintf("Subset n_chan (%d) > n_chan (%d)", 
        sub->nchan, super->nchan);
    return false;
  }

  const double sub_chbw = sub->bandwidth / (double)sub->nchan;
  const double super_chbw = super->bandwidth / (double)super->nchan;

  // If channel BWs don't match, return false for now.
  if (fabs((sub_chbw-super_chbw)/super_chbw) > bw_tol) {
    reason = stringprintf("Subset chan_bw (%f) != chan_bw (%f)", 
        sub_chbw, super_chbw);
    return false;
  }

  // Determining channel frequencies from freq and BW is not
  // reliable so we need to load the archives to check the 
  // actual freq arrays.  In future may be better to pass an
  // array via the database Entry class
  Reference::To<Archive> super_arch, sub_arch;
  super_arch = Archive::load(super->get_filename());
  sub_arch = Archive::load(sub->get_filename());
  
  // Set fractional freq tolerance to either 1% of channel BW
  // or the default, whichever is larger.
  double freq_tol_match = fabs(1e-2*sub_chbw/get_channel_frequency(sub_arch,0));
  if (freq_tol > freq_tol_match) { freq_tol_match = freq_tol; }

  // Loop over "sub" chans, make sure they all exist in super
  unsigned n_matched=0;
  for (unsigned i=0; i<sub->nchan; i++) {
    double sub_freq = get_channel_frequency(sub_arch, i);
    for (unsigned j=0; j<super->nchan; j++) {
      double super_freq = get_channel_frequency(super_arch, j);
      if (fabs((sub_freq-super_freq)/super_freq) < freq_tol_match) {
        n_matched++;
        break;
      } 
    }
  }

  if (n_matched==sub->nchan)
    return true;
  else {
    reason = stringprintf("Only matched %u/%u channels (database n1=%s n2=%s)", 
        n_matched, sub->nchan, super->get_filename().c_str(), 
        sub->get_filename().c_str());
    return false;
  }
}

int ChannelSubsetMatch::super_channel (const Archive* super,
				       const Archive* sub, int subchan) try
{
  double freq = sub->get_Integration(0)->get_centre_frequency(subchan);
  return match_channel (super->get_Integration(0), freq);
			
}
catch (Error& error)
{
  return -1;
}

int ChannelSubsetMatch::sub_channel (const Archive* super,
				     const Archive* sub, int superchan) try
{
  double freq = super->get_Integration(0)->get_centre_frequency(superchan);
  return match_channel (sub->get_Integration(0), freq);
}
catch (Error& error)
{
  return -1;
}
