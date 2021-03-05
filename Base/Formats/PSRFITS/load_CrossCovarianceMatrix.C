/***************************************************************************
 *
 *   Copyright (C) 2016 by Aditya Parthasarathy & Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/CrossCovarianceMatrix.h"

#include "psrfitsio.h"
#include "FITSError.h"

#include <stdlib.h> 

using namespace std;

void Pulsar::FITSArchive::load_CrossCovarianceMatrix (fitsfile* fptr) try
{
  int status = 0;

  if (verbose > 2)
    cerr << "FITSArchive::load_CrossCovarianceMatrix entered" << endl;
   
  // Move to COV_MAT HDU   
  fits_movnam_hdu (fptr, BINARY_TBL, "COV_MAT", 0, &status);
  
  if (status == BAD_HDU_NUM)
  {
    if (verbose > 2)
      cerr << "Pulsar::FITSArchive::load_CrossCovarianceMatrix : no COV_MAT HDU"
	   << endl;
    return;
  }

  if (status != 0)
    throw FITSError (status, "FITSArchive::load_CrossCovarianceMatrix",
		     "fits_movnam_hdu COV_MAT");
 
  Reference::To<CrossCovarianceMatrix> covar = new CrossCovarianceMatrix;
    
  // Get nbin from COV_MAT 
  int nbin = 0;
  psrfits_read_key (fptr, "NBIN", &nbin, 0, verbose > 2);

  if (!nbin)
  {
    if (verbose > 2)
      cerr << "FITSArchive::load_CrossCovarianceMatrix"
	" COV_MAT HDU contains no data." << endl;
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
      cerr << "FITSArchive::load_CrossCovarianceMatrix"
	" COV_MAT HDU contains no data" << endl;
    return;
  }

  // Set npol 
  covar->set_npol( npol ); 

  // NLAG is optional (default to 1 in older files)
  int nlag = 1;
  psrfits_read_key (fptr, "NLAG", &nlag, 1, verbose > 2);
  covar->set_nlag( nlag ); 

  covar->resize_data();

  if (verbose > 2) 
    cerr << "FITSArchive::load CrossCovarianceMatrix"
      " nbin=" << covar->get_nbin() <<
      " npol=" << covar->get_npol() <<
      " nlag=" << covar->get_nlag() <<
      " ndat=" << covar->get_data().size() << endl;

  psrfits_read_col (fptr, "DATA", covar->get_data());
    
  if (verbose > 2)
    cerr << "FITSArchive::load_CrossCovarianceMatrix loaded" << endl;

  add_extension (covar);

  if (verbose > 2)
    cerr << "FITSArchive::load_CrossCovarianceMatrix exiting" << endl;	  
}

catch (Error& error)
{
  throw error += "FITSArchive::load_CrossCovarianceMatrix";
}
