/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/Pointing.h"
#include "Pulsar/FITSHdrExtension.h"

#include "FITSError.h"
#include "psrfitsio.h"
#include "templates.h"

#include <float.h>

using namespace std;

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////
//! A function to write an integration to a row in a FITS file on disk.

void Pulsar::FITSArchive::unload_Integration (int row, 
                                              const Integration* integ, 
                                              fitsfile* thefptr) const
{
  const IntegrationOrder* order = get<IntegrationOrder>();
  if (order)
  {
    if (verbose > 2)
      cerr << "FITSArchive::unload_integration order=" 
           << order->get_extension_name() << endl;

    psrfits_write_col (thefptr, "INDEXVAL", row, order->get_Index(row-1));
  }

  // Set the duration of the integration
  psrfits_write_col (thefptr, "TSUBINT", row, integ->get_duration());
  
  // Set the start time of the integration

  if (verbose > 2)
    cerr << "FITSArchive::unload_integration row=" << row
         << " epoch=" << integ->get_epoch () << endl;

  double time = (integ->get_epoch () - reference_epoch).in_seconds();

  psrfits_write_col (thefptr, "OFFS_SUB", row, time);

  if (verbose > 2)
    cerr << "FITSArchive::unload_integration row=" << row 
         << " OFFS_SUB = " << time << " written" << endl;

  // Write other useful info
  
  const Pointing* theExt = integ->get<Pointing>();
  if (theExt)
    unload(thefptr,theExt,row);

  // Write folding period if predictor model does not exist
  if (!has_model())
    psrfits_write_col(thefptr, "PERIOD", row, integ->get_folding_period());

  // Write the channel centre frequencies

  vector<float> temp (nchan);

  for (unsigned j = 0; j < nchan; j++)
    temp[j] = integ->get_centre_frequency(j);

  psrfits_write_col (thefptr, "DAT_FREQ", row, temp, vector<unsigned> ());

  // Write the profile weights

  for (unsigned j = 0; j < nchan; j++)
    temp[j] = integ->get_weight(j);

  psrfits_write_col (thefptr, "DAT_WTS", row, temp, vector<unsigned> ());

  // Start writing profiles
  
  if (verbose > 2)
    cerr << "FITSArchive::unload_integration writing profiles" << endl;

  // Resize the FITS column arrays

  // 20/02/2003 This is unnecessary. I do it before I call the function.
  // This section is only included to help avoid confusion (including
  // my own!)

  /*
    
    colnum = 0;
    fits_get_colnum (thefptr, CASEINSEN, "DAT_OFFS", &colnum, &status);
    fits_modify_vector_len (thefptr, colnum, (nchan*npol), &status);
    
    colnum = 0;
    fits_get_colnum (thefptr, CASEINSEN, "DAT_WTS", &colnum, &status);
    fits_modify_vector_len (thefptr, colnum, (nchan*npol), &status);
    
    colnum = 0;
    fits_get_colnum (thefptr, CASEINSEN, "DAT_SCL", &colnum, &status);
    fits_modify_vector_len (thefptr, colnum, (nchan*npol), &status);
    
    colnum = 0;
    fits_get_colnum (thefptr, CASEINSEN, "DATA", &colnum, &status);
    fits_modify_vector_len (thefptr, colnum, (nchan*npol*nbin), &status);

  */
  
  int counter1 = 1;
  int counter2 = 1;

  const Profile *p = 0;

  vector<int16> temparray2 (nbin);

  float min = 0.0;
  float max = 0.0;
  float offset = 0.0;
  float scalefac = 0.0;  
  
  bool save_signed = true;

  float max_short;

  if (save_signed)
    max_short = pow(2.0,15.0)-1.0;
  else
    max_short = pow(2.0,16.0)-1.0;

  for(unsigned a = 0; a < npol; a++) {
    for(unsigned b = 0; b < nchan; b++) {
      
      p = integ->get_Profile(a,b);
      vector<float> temparray1 (nbin);
      for(unsigned j = 0; j < get_nbin(); j++)
        temparray1[j] = p->get_amps()[j];
      
      if (verbose > 2) {
        cerr << "FITSArchive::unload_integration got profile" << endl;
        cerr << "nchan = " << b << endl;
        cerr << "npol  = " << a << endl;
      }

      minmax(temparray1, min, max);

      if (save_signed)
        offset = 0.5 * (max + min);
      else
        offset = min;

      if (verbose > 2)
        cerr << "FITSArchive::unload_integration offset = "
             << offset
             << endl;
      
      scalefac = 1.0;
      
      // Test for dynamic range
      if (fabs(min - max) < (100.0 * FLT_MIN)) {
        if (verbose > 2) {
          cerr << "FITSArchive::unload_integration WARNING no range in profile"
               << endl;
        }
      }
      else
        // Find the scale factor
        scalefac = (max - min) / max_short;
      
      if (verbose > 2)
        cerr << "FITSArchive::unload_integration scalefac = "
             << scalefac
             << endl;
      
      // Apply the scale factor
      
      for (unsigned i = 0; i < nbin; i++) {
        temparray2[i] = int16 ((temparray1[i]-offset) / scalefac);
      }

      // Write the offset to file

      if (verbose > 2)
        cerr << "FITSArchive::unload_integration writing offset" << endl;

      int status = 0; 
      int colnum = 0;
      fits_get_colnum (thefptr, CASEINSEN, "DAT_OFFS", &colnum, &status);

      if (status != 0)
        throw FITSError (status, "FITSArchive:unload_integration",
                         "fits_get_colnum DAT_OFFS");

      fits_write_col (thefptr, TFLOAT, colnum, row, counter1, 1, 
                      &offset, &status);
      
      if (status != 0)
        throw FITSError (status, "FITSArchive:unload_integration",
                         "fits_write_col DAT_OFFS");

      // Write the scale factor to file

      if (verbose > 2)
        cerr << "FITSArchive::unload_integration writing scale fac" << endl;

      colnum = 0;
      fits_get_colnum (thefptr, CASEINSEN, "DAT_SCL", &colnum, &status);

      if (status != 0)
        throw FITSError (status, "FITSArchive:unload_integration",
                         "fits_get_colnum DAT_SCL");

      fits_write_col (thefptr, TFLOAT, colnum, row, counter1, 1, 
                      &scalefac, &status);
      
      if (status != 0)
        throw FITSError (status, "FITSArchive:unload_integration",
                         "fits_write_col DAT_SCL");
      
      counter1 ++;

      // Write the data

      if (verbose > 2)
        cerr << "FITSArchive:unload_integration writing data" << endl;

      colnum = 0;
      fits_get_colnum (thefptr, CASEINSEN, "DATA", &colnum, &status);

      if (status != 0)
        throw FITSError (status, "FITSArchive:unload_integration",
                         "fits_get_colnum DATA");

      fits_write_col (thefptr, TSHORT, colnum, row, counter2, nbin, 
                      &(temparray2[0]), &status);

      if (status != 0)
        throw FITSError (status, "FITSArchive:unload_integration",
                         "fits_write_col DATA");

      counter2 = counter2 + nbin;
        
      if (verbose > 2)
        cerr << "FITSArchive:unload_integration looping" << endl;
      
    }     
  }
  
  
  if (verbose > 2)
    cerr << "FITSArchive::unload_integration finished" << endl;
}

//
// End of unload_integration function
// //////////////////////////////////
// //////////////////////////////////
