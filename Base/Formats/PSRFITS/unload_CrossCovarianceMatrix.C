/***************************************************************************
 *
 * unload_CrossCovarianceMatrix :: For storing covariance matrix data in COV_MAT PSRFITS Binary Table
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h" 
#include "Pulsar/CrossCovarianceMatrix.h" 

#include "psrfitsio.h" 
#include <stdlib.h>  

using namespace std;

void Pulsar::FITSArchive::unload (fitsfile* fptr,
				  const CrossCovarianceMatrix* covar) 
{
  if (verbose > 2)
    cerr << "FITSArchive::unload CrossCovarianceMatrix entered" << endl;
    
  // Move and Clear existing rows in COV_MAT 
  psrfits_move_hdu (fptr, "COV_MAT");

  if (verbose > 2) 
    cerr << "FITSArchive::unload CrossCovarianceMatrix"
      " nbin=" << covar->get_nbin() <<
      " npol=" << covar->get_npol() <<
      " nlag=" << covar->get_nlag() <<
      " ndat=" << covar->get_data().size() << endl;
    
  // Update nbin value in NBIN (COV_MAT)
  psrfits_update_key (fptr, "NBIN", covar->get_nbin());
  psrfits_update_key (fptr, "NPOL", covar->get_npol());
  psrfits_update_key (fptr, "NLAG", covar->get_nlag());

  // Write Matrix data into the DATA (COV_MAT) column

  if (covar->has_stream())
  {
    psrfits_write_col (fptr, "DATA", 1, covar->get_stream());
    covar->get_stream()->verify_end_of_data ();
  }
  else
  {
    vector<unsigned> dimensions;  
    psrfits_write_col (fptr, "DATA", 1, covar->get_data(), dimensions);
  }

  if (verbose > 2)       
    cerr << "FITSArchive::unload CrossCovarianceMatrix exiting" << endl;
}

