/***************************************************************************
 *
 * //load_CovarianceMatrix.C :: For reading covariance matrix data from the COV_MAT PSRFITS Binary table
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
      if (verbose > 2) cerr << "Pulsar::FITSArchive::load_CovarianceMatrix : no COV_MAT HDU" << endl;
      return;
    }

  if (status != 0)
      throw FITSError (status, "FITSArchive::load_CovarianceMatrix : fits_movnam_hdu COV_MAT");
 
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
  
	// Read Matrix Data from MAT_ONE binary table (COV_MAT)
  vector<double> covariance_in( nbin*nbin );
	psrfits_read_col (fptr, "MAT_ONE", covariance_in);
    
  if (verbose > 2)
    cerr << "FITSArchive::load_CovarianceMatrix : Matrix data read" << endl;

  double *dest = (double *)malloc(nbin*nbin*sizeof(double)); 
  for(int i=0;i<nbin*nbin;i++) 		  
      dest[i] = covariance_in.at(i);

	// Set Matrix
  covar->set_matrix(dest);
  
  add_extension (covar);

  free (dest);

  if (verbose > 2)
    cerr << "FITSArchive::load_CovarianceMatrix exiting" << endl;	  
}

catch (Error& error)
   {
     throw error += "FITSArchive::load_CovarianceMatrix";  // Check out the FITSError.h file (for any required inclusions)
   }