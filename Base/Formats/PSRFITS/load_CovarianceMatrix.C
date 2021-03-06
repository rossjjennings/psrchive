/***************************************************************************
 *
 *   Copyright (C) 2016 by Aditya Parthasarathy & Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/CovarianceMatrix.h"

#include "psrfitsio.h"
#include "FITSError.h"

#include <stdlib.h> 

using namespace std;

void Pulsar::FITSArchive::load_CovarianceMatrix (fitsfile* fptr) 
try
{
  int status = 0;

  if (verbose > 2)
    cerr << "FITSArchive::load_CovarianceMatrix entered" << endl;
   
  // Move to COV_MAT HDU   
  fits_movnam_hdu (fptr, BINARY_TBL, "COV_MAT", 0, &status);
  
  if (status == BAD_HDU_NUM)
  {
    if (verbose > 2)
      cerr << "Pulsar::FITSArchive::load_CovarianceMatrix : no COV_MAT HDU"
	   << endl;
    return;
  }

  if (status != 0)
    throw FITSError (status, "FITSArchive::load_CovarianceMatrix",
		     "fits_movnam_hdu COV_MAT");
 
  Reference::To<CovarianceMatrix> covar = new CovarianceMatrix;
    
  // Get nbin from COV_MAT 
  int nbin = 0;
  psrfits_read_key (fptr, "NBIN", &nbin, 0, verbose > 2);

  if (!nbin)
  {
    if (verbose > 2)
      cerr << "FITSArchive::load_CovarianceMatrix COV_MAT HDU : contains no data. CovarianceMatrix not loaded" << endl;
    return;
  }
  
  // Set nbin 
  covar->set_nbin( nbin ); 

  // Get npol from COV_MAT 
  int npol = 0;
  psrfits_read_key (fptr, "NPOL", &npol, 0, verbose > 2);

  if (!npol)
  {
    if (verbose > 2)
      cerr << "FITSArchive::load_CovarianceMatrix COV_MAT HDU : contains no data. CovarianceMatrix not loaded" << endl;
    return;
  }
  // Set npol 
  covar->set_npol( npol ); 

  covar->resize();
  psrfits_read_col (fptr, "DATA", covar->get_data());
    
  if (verbose > 2)
    cerr << "FITSArchive::load_CovarianceMatrix : Matrix data read" << endl;

  add_extension (covar);

  if (verbose > 2)
    cerr << "FITSArchive::load_CovarianceMatrix exiting" << endl;	  
}

catch (Error& error)
   {
     throw error += "FITSArchive::load_CovarianceMatrix";
   }
