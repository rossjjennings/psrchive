/***************************************************************************
 *
 *   Copyright (C) 2011 by Andrew Jameson
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSSKLoader.h"
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
                     "fits_movnam_hdu SPECKURT");

  // setup loader with info to retreive SK data on demand
  Reference::To<Pulsar::SpectralKurtosis> sk = new SpectralKurtosis;

  Reference::To<Pulsar::FITSArchive::SKLoader> loader = new SKLoader;

  // TODO storing the SPECKURT hdu_num might be a future optimization
  int hdu_num = 0;

  loader->init (read_fptr, row, hdu_num);

  sk->set_loader (loader);

  integ->add_extension (sk);

  if (verbose == 3)
    cerr << "FITSArchive::load_SpectralKurtosis exiting" << endl;

}
catch (Error& error)
{
  throw error += "FITSArchive::load_SpectralKurtosis";
}
