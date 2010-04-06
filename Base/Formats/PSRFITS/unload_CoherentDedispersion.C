/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/CoherentDedispersion.h"
#include "psrfitsio.h"

using namespace std;

template<typename T, class C, class Method>
void pack (std::vector<T>& result, const C& ext, unsigned n, Method get)
{
  result.resize(n);
  bool all_equal = true;

  for (unsigned i=0; i<n; i++)
  {
    result[i] = (ext.get_output(i).*get)();
    if (result[i] != result[0])
      all_equal = false;
  }

  if (all_equal)
    result.resize(1);
}


template<typename T, class Method>
void output (fitsfile* fptr, const char* colname,
	     const Pulsar::CoherentDedispersion::InputChannel& ext,
	     unsigned ichan, Method get)
{
  vector<T> data;
  pack (data, ext, ext.get_nchan_output(), get);
  vector<unsigned> dimensions (1, data.size());

  psrfits_write_col (fptr, colname, ichan+1, data, dimensions);
}

void Pulsar::FITSArchive::unload (fitsfile* fptr,
				  const CoherentDedispersion* ext)
{
  if (verbose)
    cerr << "Pulsar::FITSArchive::unload CoherentDedispersion" << endl;

  string domain = tostring (ext->get_domain());
  psrfits_update_key (fptr, "DOMAIN", domain);
  psrfits_update_key (fptr, "CHRPTYPE", ext->get_description());

  psrfits_update_key (fptr, "DM", ext->get_dispersion_measure());
  psrfits_update_key (fptr, "DOPPLER", ext->get_doppler_correction());

  psrfits_update_key (fptr, "DATANBIT", ext->get_nbit_data());
  psrfits_update_key (fptr, "CHRPNBIT", ext->get_nbit_chirp());

  unsigned nchan = ext->get_nchan_input();
  psrfits_update_key (fptr, "NCHAN", nchan);

  psrfits_set_rows (fptr, nchan);

  typedef Pulsar::CoherentDedispersion::InputChannel InputChannel;
  typedef Pulsar::CoherentDedispersion::OutputChannel OutputChannel;

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    const InputChannel& input = ext->get_input(ichan);

    psrfits_write_col (fptr, "FREQ", ichan+1, input.get_centre_frequency());
    psrfits_write_col (fptr, "BW", ichan+1, input.get_bandwidth());
    psrfits_write_col (fptr, "OUT_NCHAN", ichan+1, input.get_nchan_output());

    output<double> (fptr, "OUT_FREQ", input, ichan,
		    &OutputChannel::get_centre_frequency);
    output<double> (fptr, "OUT_BW", input, ichan,
		    &OutputChannel::get_bandwidth);
    
    output<unsigned> (fptr, "NCHIRP", input, ichan,
		      &OutputChannel::get_nsamp);
    output<unsigned> (fptr, "NCYC_POS", input, ichan,
		      &OutputChannel::get_nsamp_overlap_pos);
    output<unsigned> (fptr, "NCYC_NEG", input, ichan,
		      &OutputChannel::get_nsamp_overlap_neg);
  }
}

