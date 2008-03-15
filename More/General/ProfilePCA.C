/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfilePCA.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Integration.h"

#include <gsl/gsl_eigen.h>

#include "Warning.h"
#include "FTransform.h"

using namespace std;

static Warning warn;

//! Default constructor
Pulsar::ProfilePCA::ProfilePCA ()
{
  nharm_cov=0;
  nharm_pca=0;
  cov=NULL;
  mean=NULL;
  wt_sum=0.0;
  wt2_sum=0.0;
  pc_values.resize(0);
  pc_vectors=NULL;
}

//! Destructor
Pulsar::ProfilePCA::~ProfilePCA()
{
  if (cov!=NULL) delete [] cov;
  if (mean!=NULL) delete [] mean;
  if (pc_vectors!=NULL) delete [] pc_vectors;
}

void Pulsar::ProfilePCA::set_nharm_cov(unsigned nharm) 
{

  // Check if nharm has been set before
  if (nharm_cov>0 && wt_sum>0.0) {
    // TODO: Implement resizing of cov matrix.  for now,
    // throw an error.
    throw Error (InvalidParam, "Pulsar::ProfilePCA::set_nharm_cov",
        "Resizing covariance matrix not yet implemented.");
  }

  // Alloc matrices
  nharm_cov = nharm;
  cov = new double[4*nharm_cov*nharm_cov]; // No DC terms
  mean = new double[2*nharm_cov + 2];      // Includes DC
}

unsigned Pulsar::ProfilePCA::get_nharm_cov()
{
  return(nharm_cov);
}

void Pulsar::ProfilePCA::set_nharm_pca(unsigned nharm)
{
  nharm_pca = nharm;
}

unsigned Pulsar::ProfilePCA::get_nharm_pca()
{
  return(nharm_pca);
}

void Pulsar::ProfilePCA::reset()
{
  wt_sum=0.0;
  wt2_sum=0.0;
  mean[0]=mean[1]=0.0;
  for (unsigned i=0; i<2*nharm_cov; i++) 
    mean[i+2]=0.0;
  for (unsigned i=0; i<4*nharm_cov*nharm_cov; i++)
    cov[i]=0.0;
}

void Pulsar::ProfilePCA::add_Profile(const Profile *p) 
{
  // Check that we have enough time resolution
  // If not, continue with a warning.
  unsigned nfbins = p->get_nbin() + 2;
  if (nfbins-2<2*nharm_cov) 
    warn << "Profile::PCA::add_Profile WARNING " 
      <<"Not enough profile bins for requested number of harmonics." 
      << endl;

  // FFT input profile
  // If we'll be adding a lot of profiles, it might
  // be more efficient to calc time domain cov matrix then
  // FFT it.  This is more straightforward, tho.
  float *fprof = new float[nfbins];
  FTransform::frc1d(p->get_nbin(), fprof, p->get_amps());
  
  // Integrate into cov matrix, mean prof
  // cov indices are offset by 2 since we're ignoring
  // the DC terms.
  unsigned lim = (nfbins-2<2*nharm_cov) ? nfbins-2 : 2*nharm_cov;
  float wt = p->get_weight();
  for (unsigned i=2; i<=lim; i++) {
    mean[i] += wt*fprof[i];
    for (unsigned j=i; j<=lim; j++) {
      cov[2*nharm_cov*(i-2) + (j-2)] += wt*fprof[i]*fprof[j];
    }
  }

  // Free mem
  delete [] fprof;

  // Increment weight sums
  wt_sum += wt;
  wt2_sum += wt*wt;
}

void Pulsar::ProfilePCA::add_Integration(const Integration *integ)
{
  // Loop over channels
  // Just take pol 0 for now
  for (unsigned ichan=0; ichan<integ->get_nchan(); ichan++) {
    add_Profile(integ->get_Profile(0,ichan));
  }
}

void Pulsar::ProfilePCA::compute()
{
  // If no data added, throw Error
  if (wt_sum==0.0)
    throw Error (InvalidState, "Pulsar::ProfilePCA::compute",
        "No data added before calling compute (weight=0.0)");

  // Convert full cov matrix to a (potentially) reduced size pca matrix
  double *pca = new double[4*nharm_pca*nharm_pca];
  for (unsigned i=0; i<2*nharm_pca; i++) {
    for (unsigned j=i; j<2*nharm_pca; j++) {
      pca[2*i*nharm_pca + j] = cov[2*i*nharm_cov + j]/wt_sum
        - mean[i+2]*mean[j+2]/(wt_sum*wt_sum);
    }
  }

  // Mirror pca matrix (prob not really necessary)
  for (unsigned i=0; i<2*nharm_pca; i++) {
    for (unsigned j=0; j<i; j++) {
      pca[2*i*nharm_pca + j] = pca[2*j*nharm_pca + i];
    }
  }

  // Run eigenvalue/vector routine (gsl)
  gsl_matrix_view m = gsl_matrix_view_array(pca, 2*nharm_pca, 2*nharm_pca);
  gsl_vector *eval = gsl_vector_alloc(2*nharm_pca);
  gsl_matrix *evec = gsl_matrix_alloc(2*nharm_pca, 2*nharm_pca);
  gsl_eigen_symmv_workspace *w = gsl_eigen_symmv_alloc(2*nharm_pca);
  gsl_eigen_symmv(&m.matrix, eval, evec, w);
  gsl_eigen_symmv_free(w);
  gsl_eigen_symmv_sort(eval, evec, GSL_EIGEN_SORT_VAL_DESC);
  
  // Organize results
  pc_values.resize(2*nharm_pca);
  for (unsigned i=0; i<2*nharm_pca; i++) {
    pc_values[i] = gsl_vector_get(eval, i);
  }
  pc_vectors = new double[4*nharm_pca*nharm_pca];
  for (unsigned i=0; i<2*nharm_pca; i++) {
    for (unsigned j=0; j<2*nharm_pca; j++) {
      pc_vectors[2*i*nharm_pca + j] = gsl_matrix_get(evec, j, i);
    }
  }

  // Free temp mem
  delete [] pca;
  gsl_matrix_free(evec);
  gsl_vector_free(eval);
}

double Pulsar::ProfilePCA::get_pc_value(unsigned i)
{
  if (i>2*nharm_pca)
    throw Error (InvalidParam, "Pulsar::ProfilePCA::get_pc_value",
        "requested out of range component (%d)", i);

  if (pc_values.size()==0) 
    compute();

  return(pc_values[i]);
}

Pulsar::Profile*
Pulsar::ProfilePCA::get_pc_vector(unsigned i, unsigned nbin) 
{
  if (i>2*nharm_pca)
    throw Error (InvalidParam, "Pulsar::ProfilePCA::get_pc_vector",
        "requested out of range component (%d)", i);

  if (pc_values.size()==0) 
    compute();

  float *fprof = new float[nbin+2];
  for (int ii=0; ii<nbin+2; ii++) fprof[ii]=0.0;
  for (unsigned ii=0; ii<2*nharm_pca; ii++) {
    fprof[ii+2] = pc_vectors[2*i*nharm_pca + ii];
  }
  Profile *prof = new Profile(nbin);
  FTransform::bcr1d(nbin, prof->get_amps(), fprof);
  // TODO : set other Profile attributes?
  delete [] fprof;
  return(prof);
}

std::vector<double> 
Pulsar::ProfilePCA::decompose(const Profile *p, unsigned n_pc)
{
  if (n_pc>2*nharm_pca)
    throw Error (InvalidParam, "Pulsar::ProfilePCA::decompose",
        "requested out of range component (%d)", n_pc);

  if (pc_values.size()==0) 
    compute();

  // FFT input profile
  unsigned nfbins = p->get_nbin() + 2;
  float *fprof = new float[nfbins];
  FTransform::frc1d(p->get_nbin(), fprof, p->get_amps());

  std::vector<double> result;
  result.resize(n_pc);
  unsigned limit = (nfbins-2 < 2*nharm_pca) ? nfbins-2 : 2*nharm_pca;
  for (unsigned ipc=0; ipc<n_pc; ipc++) {
    result[ipc] = 0.0;
    for (unsigned ibin=0; ibin<limit; ibin++) {
      result[ipc] += fprof[ibin-2] * pc_vectors[2*ipc*nharm_pca + ibin];
    }
  }

  return(result);
}

void Pulsar::ProfilePCA::unload(const std::string& filename)
{
  throw Error (InvalidState, "Pulsar::ProfilePCA::unload"
      "Unload not implemented yet.");
}

Pulsar::ProfilePCA* Pulsar::ProfilePCA::load(const std::string& filename)
{
  throw Error (InvalidState, "Pulsar::ProfilePCA::load"
      "Load not implemented yet.");
}
