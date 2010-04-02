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
void pack (std::vector<T>& result, const C* ext, unsigned n, Method get)
{
  result.resize(n);
  bool all_equal = true;

  for (unsigned i=0; i<n; i++)
  {
    result[i] = (ext->*get)(i);
    if (result[i] != result[0])
      all_equal = false;
  }

  if (all_equal)
    result.resize(1);
}


template<typename T, class Method>
void output (fitsfile* fptr, const char* colname,
	     const Pulsar::CoherentDedispersion* ext,
	     unsigned nchan, Method get)
{
  vector<T> data;
  pack (data, ext, nchan, get);
  vector<unsigned> dimensions (1, data.size());

  psrfits_write_col (fptr, colname, 1, data, dimensions);
}

void Pulsar::FITSArchive::unload (fitsfile* fptr,
				  const CoherentDedispersion* ext)
{
  string domain = tostring (ext->get_domain());
  psrfits_update_key (fptr, "DOMAIN", domain);
  psrfits_update_key (fptr, "CHRPTYPE", ext->get_description());

  psrfits_update_key (fptr, "DM", ext->get_dispersion_measure());
  psrfits_update_key (fptr, "DOPPLER", ext->get_doppler_correction());

  psrfits_update_key (fptr, "NCHAN_I", ext->get_nchan_input());
  psrfits_update_key (fptr, "NCHAN_O", ext->get_nchan_output());

  psrfits_update_key (fptr, "DATANLEV", ext->get_nlev_data());
  psrfits_update_key (fptr, "CHRPNLEV", ext->get_nlev_chirp());

  output<double> (fptr, "IN_FREQ", ext, ext->get_nchan_input(),
		  &CoherentDedispersion::get_centre_frequency_input);
  output<double> (fptr, "IN_BW", ext, ext->get_nchan_input(),
		  &CoherentDedispersion::get_bandwidth_input);

  output<double> (fptr, "OUT_FREQ", ext, ext->get_nchan_output(),
		  &CoherentDedispersion::get_centre_frequency_output);
  output<double> (fptr, "OUT_BW", ext, ext->get_nchan_output(),
		  &CoherentDedispersion::get_bandwidth_output);

  output<unsigned> (fptr, "NCHIRP", ext, ext->get_nchan_output(),
		    &CoherentDedispersion::get_nsamp);
  output<unsigned> (fptr, "NCYC_POS", ext, ext->get_nchan_output(),
		    &CoherentDedispersion::get_nsamp_overlap_pos);
  output<unsigned> (fptr, "NCYC_NEG", ext, ext->get_nchan_output(),
		    &CoherentDedispersion::get_nsamp_overlap_neg);
}
