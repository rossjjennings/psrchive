/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/Passband.h"
#include "psrfitsio.h"
#include "templates.h"

using namespace std;

void Pulsar::FITSArchive::unload (fitsfile* fptr, const Passband* bandpass) try
{
  int status = 0;

  if (verbose == 3)
    cerr << "FITSArchive::unload_passband entered" << endl;
  
  // Move to the BANDPASS HDU
  
  fits_movnam_hdu (fptr, BINARY_TBL, "BANDPASS", 0, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::bandpass::unload", 
		     "fits_movnam_hdu BANDPASS");
  
  psrfits_clean_rows (fptr);

  // Initialise a new row
  
  fits_insert_rows (fptr, 0, 1, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::bandpass::unload", 
		     "fits_insert_rows BANDPASS");

  char* comment = 0;
  
  // Write NCH_ORIG
  
  int nch_orig = bandpass->get_nchan();
  fits_update_key (fptr, TINT, "NCH_ORIG", &nch_orig, comment, &status);

  int npol = bandpass->get_npol();
  fits_update_key (fptr, TINT, "BP_NPOL", &npol, comment, &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive::bandpass::unload", 
		     "fits_update_key NCH_ORIG BP_NPOL");

  float min = 0;
  float max = 0;
  float max_int = pow(2.0,15.0)-1.0;
  
  vector<float> data_offsets (npol);
  vector<float> data_scales (npol);
  
  // Calculate the quantities
  
  for (int i = 0; i < npol; i++) {
    minmax(bandpass->get_passband(i), min, max);
    data_offsets[i] = 0.5 * (max + min);
    data_scales[i] = ((max - min) / max_int) / 2.0;
    if (data_scales[i] == 0.0) data_scales[i] = 1.0;
  }
  
  if (verbose == 3)
    cerr << "FITSArchive::unload_passband scales & offsets calculated" << endl;
  
  // Write the data offsets
  int row = 1;
  vector<unsigned> dims;

  psrfits_write_col (fptr, "DAT_OFFS", row, data_offsets, dims);
  
  if (verbose == 3)
    cerr << "FITSArchive::unload_passband offsets written" << endl;

  // Write the data scale factors
  psrfits_write_col (fptr, "DAT_SCL", row, data_scales, dims);
  
  if (verbose == 3)
    cerr << "FITSArchive::unload_passband scales written" << endl;
  
  // Calculate the scaled numbers
  
  vector<int> data( nch_orig*npol );
  int count = 0;
  vector<float> temp;
  
  for (int i = 0; i < npol; i++) {
    temp = bandpass->get_passband(i);
    for (int j = 0; j < nch_orig; j++) {
      data[count] = (int)((temp[j]-data_offsets[i])/data_scales[i]);
      count ++;
    }
  }

  assert( count == data.size() );

  if (verbose == 3)
    cerr << "FITSArchive::unload_passband points calculated" << endl;

  // Write the data itself
  dims.resize(2);
  dims[0] = nch_orig;
  dims[1] = npol;
    
  psrfits_write_col (fptr, "DATA", row, data, dims);
  
  if (verbose == 3)
    cerr << "FITSArchive::unload_passband exiting" << endl;
}
catch (Error &error)
{
  throw error += "FITSArchive::unload(Passband)";
}

