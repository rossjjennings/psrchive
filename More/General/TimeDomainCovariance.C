/***************************************************************************
 *  *
 *  *   Copyright (C) 2011 by Stefan Oslowski
 *  *   Licensed under the Academic Free License version 2.1
 *  *
 ****************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/TimeDomainCovariance.h"
#include "Pulsar/Profile.h"

#include "Warning.h"

//#define _DEBUG 1
#include "debug.h"

#include <algorithm>
#include <cstring>
#include <cassert>

#if HAVE_GSL
#include <gsl/gsl_eigen.h>
#endif

#ifdef HAVE_CULA
#include <cula_lapack.hpp>
#include <cula_blas.hpp>
#include <cula.hpp>
#endif

using namespace std;
using namespace Pulsar;

static Warning warn;

//TODO implement first / last bin

//! Default constructor
TimeDomainCovariance::TimeDomainCovariance ()
{
  eigen_decomposed = false;
  subtract_mean = true;
  first_bin = 0;
  last_bin = 0;
}

void TimeDomainCovariance::reset ()
{
  wt_sum = 0.0;
  wt_sum2 = 0.0;
  count = 0;

  eigen_decomposed = false;
  finalized = false;

  for (unsigned i = 0; i < rank * rank; ++i)
    covariance_matrix[i] = 0.0;

  for (unsigned i = 0; i < rank; ++i)
    mean[i] = 0.0;
}

void TimeDomainCovariance::set_rank ( unsigned value )
{
  if ( rank == 0 )
    rank = value;
  else
    throw Error (InvalidState, "TimeDomainCovariance::set_rank",
		 "Rank can't be changed after it was set");

  covariance_matrix.resize (rank * rank);
  mean.resize (rank);
  reset ();
}

unsigned TimeDomainCovariance::get_rank ()
{
  return rank;
}

void TimeDomainCovariance::set_count ( unsigned value )
{
  count = value;
}

unsigned TimeDomainCovariance::get_count ()
{
  return count;
}

void TimeDomainCovariance::add_Profile ( const Profile* p )
{
  float wt = p->get_weight() ;

  add_Profile ( p, wt );
}

void TimeDomainCovariance::add_Profile ( const Profile* p, float wt )
{
  if (finalized)
    throw Error (InvalidState, "TimeDomainCovariance::addProfile",
		 "already finalized");
  
  if (wt == 0.0 )
    return;

  if (rank == 0)
    set_rank ( p->get_nbin () );
  else if (rank != p->get_nbin () )
    throw Error (InvalidParam, "TimeDomainCovariance::addProfile",
		 "nbin=%u != rank=%u", p->get_nbin(), rank);

  count += 1;
  wt_sum += wt;
  wt_sum2 += wt * wt;

  const float* fprof = p->get_amps();

  DEBUG("TimeDomainCovariance::add_Profile rank=" << rank);
  
  assert (covariance_matrix.size() == rank * rank);
  
  for (unsigned i=0; i<rank; i++)
  {
    mean[i] += wt*fprof[i];
    for (unsigned j=0; j<rank; j++)
      covariance_matrix[i*rank + j] += wt*fprof[i]*fprof[j];
  }
}

void TimeDomainCovariance::get_covariance_matrix_copy ( double* dest )
{
  finalize ();
  //TODO take the last first bin into account
  memcpy ( dest, &covariance_matrix[0], rank * rank * sizeof(double) );
}

void TimeDomainCovariance::set_covariance_matrix ( const double* src )
{
  //TODO take the last first bin into account
  memcpy ( &covariance_matrix[0], src, rank * rank * sizeof(double) );
}

double TimeDomainCovariance::get_covariance_matrix_value( unsigned i,
							  unsigned j )
{
  finalize ();
  return covariance_matrix[i*rank + j];
}

//! Set the eigenvector matrix
void TimeDomainCovariance::set_eigenvectors (const std::vector<double>& copy)
{
  eigenvectors = copy;
}

void TimeDomainCovariance::set_eigenvectors ( const double* copy )
{
  memcpy (&eigenvectors[0], copy, eigenvectors.size() * sizeof(double));
}

//! Get a copy of the eigenvector matrix
void TimeDomainCovariance::get_eigenvectors_copy ( std::vector<double>& copy )
{
  eigen ();
  copy = eigenvectors;
}


void TimeDomainCovariance::get_eigenvectors_copy ( double* dest )
{
  eigen ();
  //TODO take the last first bin into account
  memcpy ( dest, &eigenvectors[0], eigenvectors.size() * sizeof(double));
}

double TimeDomainCovariance::get_eigenvectors_value( unsigned i,
						     unsigned j )
{
  eigen ();
  return eigenvectors[i*rank + j];
}

const double* TimeDomainCovariance::get_eigenvectors_pointer ()
{
  return &eigenvectors[0];
}

//! Set the eigenvalue vector
void TimeDomainCovariance::set_eigenvalues ( const std::vector<double>& copy )
{
  eigenvalues = copy;
}

void TimeDomainCovariance::set_eigenvalues ( const double* copy )
{
  memcpy (&eigenvalues[0], copy, eigenvalues.size() * sizeof(double));
}

//! Get a copy of the eigenvalue vector
void TimeDomainCovariance::get_eigenvalues_copy ( std::vector<double>& copy )
{
  eigen ();
  copy = eigenvalues;
}

void TimeDomainCovariance::get_eigenvalues_copy ( double* dest )
{
  eigen ();
  memcpy ( dest, &eigenvalues[0], eigenvalues.size() * sizeof(double));
}

double TimeDomainCovariance::get_eigenvalue ( unsigned row )
{
  return eigenvalues [row];
}

const double* TimeDomainCovariance::get_eigenvalues_pointer ()
{
  return &(eigenvalues[0]);
}

void TimeDomainCovariance::finalize ()
{
  if ( finalized )
    return;

  if (rank == 0)
    throw Error (InvalidState, "TimeDomainCovariance::finalize", "no data");
  
  for (unsigned i=0; i<rank; i++)
  {
    mean[i] /= wt_sum;
    for (unsigned j=0; j<rank; j++)
      covariance_matrix[i*rank + j] /= wt_sum;
  }

  if (subtract_mean)
  {
    DEBUG("TimeDomainCovariance::finalize subtracting mean");
    for (unsigned i=0; i<rank; i++)
      for (unsigned j=0; j<rank; j++)
	covariance_matrix[i*rank + j] -= mean[i]*mean[j];
  }
  
  finalized = true;
}

void TimeDomainCovariance::eigen ()
{
  if (eigen_decomposed)
    return;

  DEBUG("TimeDomainCovariance::eigen call finalize");
  finalize ();

#if HAVE_GSL

  DEBUG("TimeDomainCovariance::eigen view and allocate arrays");
  gsl_matrix_view m = gsl_matrix_view_array(&covariance_matrix[0], rank, rank);
  gsl_vector *eval = gsl_vector_alloc(rank);
  gsl_matrix *evec = gsl_matrix_alloc(rank, rank);
  gsl_eigen_symmv_workspace *w = gsl_eigen_symmv_alloc(rank);

#ifdef HAVE_CULA
  
  //CULA implementation of the eigenproblem:
  vector<double> eval;
  int status = culaDsyev( 'V', 'U', nbin, covariance_matrix, rank, &(eval[0]) );

  if (status)
  {
    char buf[256];
    culaGetErrorInfoString(status, culaGetErrorInfo(), buf, 256);
    culaShutdown();
    exit(EXIT_FAILURE);
  }

  // Invert the evals (cula produces them in ascending order):
  gsl_permutation *p_reverse = gsl_permutation_calloc( nbin );
  gsl_permutation_reverse( p_reverse );
  gsl_permute_vector( p_reverse, eval );
  // transposing as CULA uses column-major while GSL uses row-major ordering
  // ..and swap the columns of the evectors, to get back the correspondence to evals
  for ( unsigned i_column = 0; i_column < rank/2; i_column++)
    gsl_matrix_swap_columns( covariance, i_column, nbin-i_column-1 );

  // copy the evecs as calculated by CULA to the gsl matrix evec:
  gsl_matrix_memcpy( evec, covariance );

#else

  DEBUG("TimeDomainCovariance::eigen gsl_eigen_symmv");
  gsl_eigen_symmv(&m.matrix, eval, evec, w);
  gsl_eigen_symmv_free(w);
  DEBUG("TimeDomainCovariance::eigen gsl_eigen_symmv_sort");
  gsl_eigen_symmv_sort(eval, evec, GSL_EIGEN_SORT_VAL_DESC);

  // Organize results
  eigenvalues.resize (rank);
  eigenvectors.resize (rank * rank);
  
  for (unsigned i=0; i<rank; i++)
  {
    eigenvalues[i] = gsl_vector_get(eval, i);

    for (unsigned j=0; j<rank; j++)
      eigenvectors[i*rank + j] = gsl_matrix_get(evec, j, i);
  }
  
  // Free temp mem
  gsl_matrix_free(evec);
  gsl_vector_free(eval);

#endif

#else

  throw Error (InvalidState, "TimeDomainCovariance::eigen",
	       "not implemented (no GSL or CULA)");
  
#endif

  eigen_decomposed = true;
}

void TimeDomainCovariance::choose_bins ( unsigned val_1, unsigned val_2 )
{
  if (val_1 < val_2)
  { 
    first_bin = val_1;
    last_bin = val_2;
  }
  else
  {
    first_bin = val_2;
    last_bin = val_1;
  }

  throw Error (InvalidState, "TimeDomainCovariance::choose_bins",
	       "calculation of covariance matrix for subset of phase bins "
	       "not implemented");

  set_rank ( last_bin - first_bin + 1 );
}

string TimeDomainCovariance::get_name () const
{
  return "TimeDomain";
}

