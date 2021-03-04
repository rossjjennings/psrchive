/***************************************************************************
 *
 * // Application to whiten pulse data using Covariance Matrix
 * 
 ***************************************************************************/

// Pulsar 
#include "Pulsar/CrossCovarianceMatrix.h"
#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/UnloadOptions.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

#include <iostream>
#include <fstream>  // For testing - File Writing
#include <iomanip> 

 //Eigen Library
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Cholesky>
#include <Eigen/Eigenvalues> 
#include <unsupported/Eigen/MatrixFunctions>

using namespace std;
using namespace Pulsar;
using namespace Eigen;

//
//! An Application for whitening of pulse profiles using Covariance Matrix
//

class psrwhite : public Application
{
public:

  //! Default constructor
  psrwhite ();

  //! Process the given archive
  void process ( Archive* );

protected:

  // add program options
  void add_options (CommandLine::Menu&);
  
  // Functions for Eigenvalue Decomposition and Transformation
  void transform_data();
  void eigenvalue_decomp();
  
 
  // Eigen Matrices 
  MatrixXd Inv_eigenVec;
  MatrixXd Inv_sqrt_eigenval;
  MatrixXd eigenvec;
  MatrixXd Sqrt_eigenVal;
  MatrixXd eigendecomp;
  MatrixXd transpose;
  MatrixXd inverse;

  // Archive
  Reference::To<Archive> cov_archive;

  // Profile
  Reference::To<Profile> profile;

  //Check
  int check;  

};

psrwhite::psrwhite ()
  : Application ("psrwhite", "Application to Whiten Data using Covariance Matrix")
{
  add( new Pulsar::StandardOptions );
  add( new Pulsar::UnloadOptions );

  check = 0; 

}

// Could be used to add options in the future
void psrwhite::add_options (CommandLine::Menu& menu)
{
  // CommandLine::Argument* arg;

  // Transform Profile Amps
  //menu.add ("");
  //menu.add ("Transforming Profile Amplitude Value - Options:");

  //arg = menu.add ( Transform, "T" );
  //arg->set_help ("Enable Transformation of Profile Amplitudes" );
  
}

void psrwhite::process (Archive* archive)
{ 
  //archive->fscrunch();
  //archive->pscrunch(); 

  cov_archive = archive;  
  
  transform_data();

}
// Whitening of the Pulsar Pulse Profiles
void psrwhite::transform_data()
{
  eigenvalue_decomp();
  
  unsigned nbin = cov_archive->get_nbin();
  unsigned nsub = cov_archive->get_nsubint();
  unsigned nchan = cov_archive->get_nchan();
  unsigned npol =cov_archive->get_npol();
  VectorXd amps (nbin); 
  VectorXd profiles_unitvariance (nbin);
  
  if(verbose)
    cerr << "Archive values obtained and variables allocated" << endl;

  // Loop to access the amplitude values in individual pulse profiles
  for (unsigned isub=0; isub < nsub; isub++)    
    for (unsigned ipol=0; ipol < npol; ipol++)
      for (unsigned ichan=0; ichan < nchan; ichan++)
      {
          if(verbose)
            cerr << "Entered into Profile" << endl;
          profile = cov_archive->get_Profile (isub, ipol, ichan);
          float *prof_amps = profile->get_amps();  
  
 // Store the profile amps in an Eigen Vector
            if(verbose)
              cerr << "Storing Profile Amplitudes in Eigen Vector" << endl;
            for(int i=0;i<nbin;i++)         
              amps[i] = prof_amps[i];     

 // The transformation of the profile using results of the eigvalue decomposition
    if (verbose)
      cerr << "Transforming profile using the results of the Eigenvalue decomposition" << endl;
            profiles_unitvariance = inverse * amps;

 // Storing the transformed array into prof_amps
    if(verbose)
      cerr << "Storing the transformed array into eigen vector" << endl;
            for(int j=0;j<nbin;j++)
              prof_amps[j] = profiles_unitvariance[j];         
     }  
}


// Computes the EigenValue Decomposition of the Covariance Matrix Data
void psrwhite::eigenvalue_decomp()
{
  if(check==0)
  {
  if(verbose)  
    cerr << "Entering Eigenvaluedecomposition" << endl;
  unsigned nbin = cov_archive->get_nbin();
  int row = nbin;
  int col = nbin; 
  int count = 0;
       
    if(verbose)
      cerr << "Variable Allocation and Archive Loading" << endl; 
      
      MatrixXd covariance (row,col); 
            
      Reference::To<Archive> data = Archive::load("covariance.rf"); // Produced by psrpca
    if(verbose)
      cerr << "FITSArchive Loaded" << endl;         
      
      if( data->get<CrossCovarianceMatrix>() )
      {   

      if(verbose)  
       cerr << "Found covar" << endl;

      CrossCovarianceMatrix* covar = data->get<CrossCovarianceMatrix>();      
      // Generate the Covariance Matrix from PSRFITS Table     
            for (int i=0; i<row;i++)
            {
            for (int j=0;j<col;j++)
              {
                covariance(i,j) = covar->get_data().at(count); 
                count ++;
              }    
            }      
      }

    if(verbose)  
      cerr << "Covariance matrix loaded from FITSArchive. Computing Eigenvalue Decomposition" << endl; 
     
     // EigenValue Decomposition of the Covariance Matrix
     SelfAdjointEigenSolver<MatrixXd> es;
     es.compute(covariance);
     
     eigenvec = es.eigenvectors(); 
     
     MatrixXd eigval = es.eigenvalues().asDiagonal();
     Sqrt_eigenVal = eigval.sqrt();     
     
      // Eigenvalue Decomposition - Whitening [Whitened_Data = (Sqrt_Eigenvalue)^-1 * (Eigenvector)^-1] 
      MatrixXd eigenvalue_inverse = Sqrt_eigenVal.inverse();
      MatrixXd eigenvec_inverse = eigenvec.inverse();
      inverse = eigenvalue_inverse*eigenvec_inverse;

      /*
      // CHOLESKY METHOD - Whitening
      L = covariance.llt().matrixL(); // compute cholesky decomp -- lower triangular
      L_transpose = L.transpose();    // compute transpose -- upper triangular
      L_inverse = L_transpose.inverse();  // Inverse for uncorrelation
      */

     check=1;

     if(verbose)
     cerr << "Decomposition is done" << endl;  
   }    
}

int main (int argc, char** argv)
{
  psrwhite program;
  return program.main (argc, argv); 
}
