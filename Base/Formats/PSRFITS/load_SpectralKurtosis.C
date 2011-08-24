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

/*!
  \pre The current HDU is the SUBINT HDU
*/
void Pulsar::FITSArchive::load_SpectralKurtosis (fitsfile* fptr, int row,
                  Pulsar::Integration* integ) try
{
  if (verbose == 3)
    cerr << "FITSArchive::loadSpectralKurtosis entered" << endl;
  

  // Move to the SPECKURT HDU
  int status = 0;
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
                     "fits_movnam_hdu FEEDPAR");

  Reference::To<SpectralKurtosis> sk = new SpectralKurtosis;

  // Get NCPAR 
  int ncpar = 0;
  psrfits_read_key (fptr, "NCPAR", &ncpar);
  if (ncpar < 0)
    ncpar = 0;
  unsigned int npol = (unsigned) ncpar;

  // Get NCHAN
  int nchan = 0;
  psrfits_read_key (fptr, "NCHAN", &nchan);

  // Allocate internal storage
  sk->resize(npol, nchan);
  
  // Get SK_INT
  unsigned int sk_int = 0;
  psrfits_read_key (fptr, "SK_INT", &sk_int);

  // Get SK_EXCIS
  unsigned sk_excis = 0;
  psrfits_read_key (fptr, "SK_EXCIS", &sk_excis);
 
  long dimension = nchan * npol;
  if (dimension == 0)
  {
    if (verbose == 3)
      cerr << "FITSArchive::load_SpectralKurtosis SPECKURT HDU"
           << " contains no data. SpectralKurtosis not loaded" << endl;
      return;
  }

  vector<float> fil_sum (dimension);
  psrfits_read_col (fptr, "FIL_SUM", fil_sum);

  vector<unsigned> fil_hits (dimension);
  psrfits_read_col (fptr, "FIL_HIT", fil_hits);

  vector<float> unfil_sum (dimension);
  psrfits_read_col (fptr, "UNFIL_SUM", unfil_sum);

  int64_t unfiltered_hits = 0;
  psrfits_read_key (fptr, "UNFIL_HIT", &unfiltered_hits);

  if (verbose == 3)
    cerr << "FITSArchive::load_SpectralKurtosis data read" << endl;

  unsigned count = 0;
  for (unsigned ichan = 0; ichan < nchan; ichan++)
  {
    for (int ipol= 0; ipol < npol; ipol++)
    {
      sk->set_filtered_sum (ichan, ipol, fil_sum[ipol*nchan + ichan]);
      sk->set_filtered_hits (ichan, ipol, fil_hits[ipol*nchan + ichan]);
      sk->set_unfiltered_sum (ichan, ipol, unfil_sum[ipol*nchan + ichan]);
      count++;
    }
  }
  sk->set_unfiltered_hits((uint64_t) unfiltered_hits);

  integ->add_extension (sk);
  
  if (verbose == 3)
    cerr << "FITSArchive::load_SpectralKurtosis exiting" << endl;

}
catch (Error& error)
{
  throw error += "FITSArchive::load SpectralKurtosis";
}
