#include <float.h>

#include "Pulsar/FITSArchive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/Pointing.h"

#include "FITSError.h"
#include "genutil.h"

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////
//! A function to write an integration to a row in a FITS file on disk.

void Pulsar::FITSArchive::unload_integration (int row, 
					      const Integration* integ, 
					      fitsfile* thefptr) const
{
  int status = 0;
 
  bool has_alt_order = false;

  if (get<Pulsar::IntegrationOrder>()) {
    has_alt_order = true;
    if (verbose == 3)
      cerr << "FITSArchive::unload_integration using " 
	   << get<Pulsar::IntegrationOrder>()->get_extension_name()
	   << endl;
  }

  // Set the subint number
  
  int colnum = 0;
  fits_get_colnum (thefptr, CASEINSEN, "ISUBINT", &colnum, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive:unload_integration",
		     "fits_get_colnum ISUBINT");
  
  fits_write_col (thefptr, TINT, colnum, row, 1, 1, &row, &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive:unload_integration",
		     "fits_write_col ISUBINT");
  
  // Write out the index values, if applicable
  if (has_alt_order) {
    colnum = 0;
    fits_get_colnum (thefptr, CASEINSEN, "INDEXVAL", &colnum, &status);

    if (status != 0)
      throw FITSError (status, "FITSArchive:unload_integration",
		       "fits_get_colnum INDEXVAL");
    
    double value = get<Pulsar::IntegrationOrder>()->get_Index(row-1);
    fits_write_col (thefptr, TDOUBLE, colnum, row, 1, 1, &value, &status);
  }

  // Set the duration of the integration
  
  colnum = 0;
  fits_get_colnum (thefptr, CASEINSEN, "TSUBINT", &colnum, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive:unload_integration",
		     "fits_get_colnum TSUBINT");
  
  double duration = integ->get_duration();  
  fits_write_col (thefptr, TDOUBLE, colnum, row, 1, 1, &duration, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive:unload_integration",
		     "fits_write_col TSUBINT");
  
  if (verbose == 3)
    cerr << "FITSArchive::unload_integration row=" << row
         << " TSUBINT = " << duration << " written" << endl;
  
  // Set the start time of the integration
    
  colnum = 0;
  fits_get_colnum (thefptr, CASEINSEN, "OFFS_SUB", &colnum, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive:unload_integration",
		     "fits_get_colnum OFFS_SUB");

  if (verbose == 3)
    cerr << "FITSArchive::unload_integration row=" << row
         << " epoch=" << integ->get_epoch () << endl;

  double time = 0.0;

  if (duration != 0)
    time = (integ->get_epoch () - reference_epoch).in_seconds();
  
  fits_write_col (thefptr, TDOUBLE, colnum, row, 1, 1, &time, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive:unload_integration",
		     "fits_write_col OFFS_SUB");
  
  if (verbose == 3)
    cerr << "FITSArchive::unload_integration row=" << row 
         << " OFFS_SUB = " << time << " written" << endl;

  // Write other useful info
  
  const Pointing* theExt = integ->get<Pointing>();
  if (theExt)
    unload(thefptr,theExt,row);
  
  // Write the channel centre frequencies

  colnum = 0;
  fits_get_colnum (thefptr, CASEINSEN, "DAT_FREQ", &colnum, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive:unload_integration",
		     "fits_get_colnum DAT_FREQ");
  
  //fits_modify_vector_len (thefptr, colnum, nchan, &status);

  vector < float >  temp_array(nchan);

  for(unsigned j = 0; j < nchan; j++)
    temp_array[j] = integ->get_centre_frequency(j);

  fits_write_col (thefptr, TFLOAT, colnum, row, 1, nchan, 
		  &(temp_array[0]), &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive:unload_integration",
		     "fits_write_col DAT_FREQ");

  // Write the profile weights

  colnum = 0;
  fits_get_colnum (thefptr, CASEINSEN, "DAT_WTS", &colnum, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive:unload_integration",
		     "fits_get_colnum DAT_WTS");

  //fits_modify_vector_len (thefptr, colnum, nchan, &status);

  for(unsigned j = 0; j < nchan; j++)
    temp_array[j] = integ->get_weight(j);

  fits_write_col (thefptr, TFLOAT, colnum, row, 1, nchan, 
		  &(temp_array[0]), &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive:unload_integration",
		     "fits_write_col DAT_WTS");
  
  // Start writing profiles
  
  if (verbose == 3)
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

  int16* temparray2 = new int16 [nbin];

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
      float* temparray1 = new float[nbin];
      for(unsigned j = 0; j < get_nbin(); j++)
	temparray1[j] = (p->get_amps())[j];
      
      if (scale_cross_products) {
	if (integ->get_state() == Signal::Coherence) {
	  if (a == 2 || a == 3)
	    for(unsigned j = 0; j < get_nbin(); j++)
	      temparray1[j] /= 2.0;
	}
      }
      
      if (verbose == 3) {
	cerr << "FITSArchive::unload_integration got profile" << endl;
	cerr << "nchan = " << b << endl;
	cerr << "npol  = " << a << endl;
      }

      minmaxval(nbin, temparray1, &min, &max);

      if (save_signed)
	offset = 0.5 * (max + min);
      else
	offset = min;

      if (verbose == 3)
	cerr << "FITSArchive::unload_integration offset = "
	     << offset
	     << endl;
      
      scalefac = 1.0;
      
      // Test for dynamic range
      if (fabs(min - max) < (100.0 * FLT_MIN)) {
	if (verbose == 3) {
	  cerr << "FITSArchive::unload_integration WARNING no range in profile"
	       << endl;
	}
      }
      else
	// Find the scale factor
	scalefac = (max - min) / max_short;
      
      if (verbose == 3)
	cerr << "FITSArchive::unload_integration scalefac = "
	     << scalefac
	     << endl;
      
      // Apply the scale factor
      
      for (unsigned i = 0; i < nbin; i++) {
	temparray2[i] = int16 ((temparray1[i]-offset) / scalefac);
      }

      // Write the offset to file

      if (verbose == 3)
	cerr << "FITSArchive::unload_integration writing offset" << endl;

      colnum = 0;
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

      if (verbose == 3)
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

      if (verbose == 3)
	cerr << "FITSArchive:unload_integration writing data" << endl;

      colnum = 0;
      fits_get_colnum (thefptr, CASEINSEN, "DATA", &colnum, &status);

      if (status != 0)
	throw FITSError (status, "FITSArchive:unload_integration",
			 "fits_get_colnum DATA");

      fits_write_col (thefptr, TSHORT, colnum, row, counter2, nbin, 
		      temparray2, &status);

      if (status != 0)
	throw FITSError (status, "FITSArchive:unload_integration",
			 "fits_write_col DATA");

      counter2 = counter2 + nbin;
	
      if (verbose == 3)
	cerr << "FITSArchive:unload_integration looping" << endl;
      
      delete[] temparray1;
    }	  
  }
  
  delete[] temparray2;
  
  if (verbose == 3)
    cerr << "FITSArchive::unload_integration finished" << endl;
}

//
// End of unload_integration function
// //////////////////////////////////
// //////////////////////////////////
