/***************************************************************************
 *
 *   Copyright (C) 2011 by Andrew Jameson
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/SpectralKurtosis.h"

#include "FITSError.h"
#include "psrfitsio.h"

using namespace std;

void Pulsar::FITSArchive::unload_sk_integrations (fitsfile* ffptr) const
{
  int status = 0;
  char* comment = 0;

  // Move to the SPECKURT Binary Table
  fits_movnam_hdu (ffptr, BINARY_TBL, "SPECKURT", 0, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_sk_integrations", 
                     "fits_movnam_hdu SPECKURT");

  // ensure npol, nchan, m and nsigma match for all integrations
  const SpectralKurtosis* ske = get_Integration (0)->get<SpectralKurtosis>();

  unsigned sk_npol  = ske->get_npol();
  unsigned sk_nchan = ske->get_nchan();
  unsigned sk_m = ske->get_M();
  unsigned sk_nsigma = ske->get_excision_threshold();

  for (unsigned i = 1; i < nsubint; i++) 
  {
    ske = get_Integration (i)->get<SpectralKurtosis>();
    if (sk_npol != ske->get_npol())
      throw FITSError (status, "FITSArchive::unload_sk_integrations",
                        "sk_npol mismatch for SPECKURT");

    if (sk_nchan != ske->get_nchan())
      throw FITSError (status, "FITSArchive::unload_sk_integrations",
                        "sk_nchan mismatch for SPECKURT");

    if (sk_m != ske->get_M())
      throw FITSError (status, "FITSArchive::unload_sk_integrations",
                        "sk_m mismatch for SPECKURT");

    if (sk_nsigma != ske->get_excision_threshold ())
      throw FITSError (status, "FITSArchive::unload_sk_integrations",
                        "sk_nsigma mismatch for SPECKURT");
  }

  // write the common parameters to file
  psrfits_update_key (ffptr, "NPOL", sk_npol);
  psrfits_update_key (ffptr, "NCHAN", sk_nchan);
  psrfits_update_key (ffptr, "SK_INT", sk_m);
  psrfits_update_key (ffptr, "SK_EXCIS", sk_nsigma);
  
  // Insert nsubint rows
  if (verbose > 2)
    cerr << "FITSArchive::unload_sk_integrations nsubint=" << nsubint << endl;

  psrfits_set_rows (ffptr, nsubint);

  // write each SK integation to file
  unsigned row = 0;

  long dimension = sk_nchan * sk_npol;
  vector<float> fil_sum (dimension);
  vector<unsigned> fil_hits (sk_nchan);
  vector<float> unfil_sum (dimension);
  int64_t unfil_hits;

  vector<unsigned> dimensions (2);
  dimensions[0] = sk_npol;
  dimensions[1] = sk_nchan;

  vector<unsigned> hitsdim (1);
  dimensions[0] = sk_nchan;

  for (unsigned i = 0; i < nsubint; i++)
  {
    ske = get_Integration (i)->get<SpectralKurtosis>();
    row = i + 1;
  
    // pack the vectors in the requried format
    for (unsigned ichan = 0; ichan < sk_nchan; ichan++)
    { 
      fil_hits[ichan] = (unsigned) ske->get_filtered_hits (ichan);

      for (int ipol= 0; ipol < sk_npol; ipol++)
      {
        fil_sum[ipol*sk_nchan + ichan] = ske->get_filtered_sum (ichan, ipol);

        unfil_sum[ipol*sk_nchan + ichan] = ske->get_unfiltered_sum (ichan, ipol);
      }
    }
    unfil_hits = ske->get_unfiltered_hits ();

    psrfits_write_col (ffptr, "FIL_SUM", row, fil_sum, dimensions);
    psrfits_write_col (ffptr, "FIL_HIT", row, fil_hits, hitsdim);
    psrfits_write_col (ffptr, "UNFIL_SUM", row, unfil_sum, dimensions);
    psrfits_write_col (ffptr, "UNFIL_HIT", row, unfil_hits);
  }

  if (verbose > 2)
    cerr << "FITSArchive::unload_sk_integrations exit" << endl;
}

