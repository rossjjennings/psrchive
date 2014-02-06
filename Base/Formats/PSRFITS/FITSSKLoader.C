/***************************************************************************
 *
 *   Copyright (C) 2011 by Andrew Jameson
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSSKLoader.h"
#include "FITSError.h"
#include "psrfitsio.h"

using namespace std;

//
// Each instance of an SKLoader corresponds to a single integrations' SK 
// statistics. It is constructed with a pointer to the fits file, and a row
// (integration number). Only if/when the SK data is needed will the load
// method be called, whereby it will use the fptr + row to gt the relevant
// values from the psrfits file and insert them into the supplied SK instance
//

Pulsar::FITSArchive::SKLoader::SKLoader ()
{
  fptr = 0;
  row = 0;
  hdu_num = 0;
}

Pulsar::FITSArchive::SKLoader::~SKLoader ()
{
}

/*! Initialize loader to be able to load data from file */
void Pulsar::FITSArchive::SKLoader::init (fitsfile * _fptr, int _row, int _hdu_num)
{
  fptr = _fptr;
  row = _row;
  hdu_num = _hdu_num;
}

/*! Load integration from PSRFITS file/row into inegration */
void Pulsar::FITSArchive::SKLoader::load (SpectralKurtosis * sk)
{
  if (verbose == 3)
    cerr << "FITSArchive::loadSpectralKurtosis entered" << endl;

  int status = 0;
  int colnum = 0;

  // Move to the SPECKURT HDU
  fits_movnam_hdu (fptr, BINARY_TBL, "SPECKURT", 0, &status);

  if (status == BAD_HDU_NUM)
  {
    if (verbose == 3)
      cerr << "Pulsar::FITSArchive::load_SpectralKurtosis no SPECKURT HDU"
           << endl;
    return;
  }

  if (status != 0)
    throw FITSError (status, "FITSArchive::load_SpectralKurtosis",
                     "fits_movnam_hdu SPECKURT");

  // Get NPOL [npol]
  unsigned npol = 0;
  psrfits_read_key (fptr, "NPOL", &npol);

  // Get NCHAN [nchan]
  unsigned nchan = 0;
  psrfits_read_key (fptr, "NCHAN", &nchan);

  // Allocate internal storage
  sk->resize(npol, nchan);

  // Get SK_INT [number of samples integrated into SK samples]
  unsigned int sk_int = 0;
  psrfits_read_key (fptr, "SK_INT", &sk_int);
  sk->set_M (sk_int);

  // Get SK_EXCIS [excision threshold in nsigma]
  unsigned sk_nsigma= 0;
  psrfits_read_key (fptr, "SK_EXCIS", &sk_nsigma);
  sk->set_excision_threshold (sk_nsigma);

  long dimension = nchan * npol;
  if (dimension == 0)
  {
    if (verbose == 3)
      cerr << "FITSArchive::load_SpectralKurtosis SPECKURT HDU"
           << " contains no data. SpectralKurtosis not loaded" << endl;
      return;
  }

  vector<float> fil_sum (dimension);
  psrfits_read_col (fptr, "FIL_SUM", fil_sum, row);

  vector<unsigned> fil_hits (nchan);
  psrfits_read_col (fptr, "FIL_HIT", fil_hits, row);

  vector<float> unfil_sum (dimension);
  psrfits_read_col (fptr, "UNFIL_SUM", unfil_sum, row);

  int64_t unfiltered_hits = 0;
  psrfits_read_col (fptr, "UNFIL_HIT", &unfiltered_hits, row);

  if (verbose == 3)
    cerr << "FITSArchive::load_SpectralKurtosis data read" << endl;

  unsigned count = 0;
  for (unsigned ichan = 0; ichan < nchan; ichan++)
  {
    sk->set_filtered_hits (ichan, fil_hits[ichan]);

    for (int ipol= 0; ipol < npol; ipol++)
    {
      sk->set_filtered_sum (ichan, ipol, fil_sum[ipol*nchan + ichan]);
      sk->set_unfiltered_sum (ichan, ipol, unfil_sum[ipol*nchan + ichan]);
      count++;
    }
  }
  sk->set_unfiltered_hits((uint64_t) unfiltered_hits);
  
}
