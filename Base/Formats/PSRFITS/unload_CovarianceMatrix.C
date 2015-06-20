/***************************************************************************
 *
 * // Unload_CovarianceMatrix :: For storing covariance matrix data in COV_MAT PSRFITS Binary Table
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h" 
#include "Pulsar/CovarianceMatrix.h" 

#include "psrfitsio.h" 
#include <stdlib.h>  

using namespace std;

void Pulsar::FITSArchive::unload (fitsfile* fptr, const CovarianceMatrix* covar) 
{
    if (verbose > 2)
         cerr << "FITSArchive::unload CovarianceMatrix entered" << endl;
    
    // Move and Clear existing rows in COV_MAT 
    psrfits_move_hdu (fptr, "COV_MAT");
    psrfits_clean_rows (fptr);    
    
    // Insert new rows for Covariance Matrix Data
    int status = 0;
    fits_insert_rows (fptr, 0, 1, &status);    
    if (status != 0)
         throw FITSError (status, "FITSArchive::unload CovarianceMatrix :: fits_insert_rows COV_MAT");
    
    // Get nbin from TDC Class
    int nbin = 1024; //covar->get_nbin(); // No get_nbin as of now
    if (verbose > 2) 
        cerr << "FITSArchive::unload CovarianceMatrix nbin = "<< nbin << endl;
    
    // Update nbin value in NBIN (COV_MAT)
    psrfits_update_key (fptr, "NBIN", nbin);

    // Allocating required variables
    double *dest = (double *)malloc(nbin*nbin*sizeof(double));
    vector<double> covariance_out;
    covariance_out.resize(nbin*nbin);
    vector<unsigned> dimensions;  
    
    // Get matrix data from TDC Class
    covar->get_matrix(dest); 

    for(int i=0;i<nbin*nbin;i++) 
        covariance_out.at(i) = dest[i];       
    
    // Write Matrix data into the MAT_ONE binary table (COV_MAT)
    psrfits_write_col (fptr, "MAT_ONE", 1, covariance_out, dimensions);

    if (verbose > 2) 
        cerr << "FITSArchive::unload CovarianceMatrix Matrix Data written" << endl;

    free(dest);

    if (verbose > 2)       
        cerr << "FITSArchive::unload CovarianceMatrix exiting" << endl;    
    
}