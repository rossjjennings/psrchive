/***************************************************************************
 *
 *   Copyright (C) 2022 - 2024 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/GaussJordan.h"
#include "Jacobi.h"
#include "random.h"
#include <vector>

using namespace std;

const unsigned dim = 37;

#define PRINT_MATRIX 0

int once ()
{
  vector< vector<double> > test (dim, vector<double>(dim));
  random_matrix (test, 10.0);

  vector< vector<double> > copy = test;

  vector< vector<double> > other (dim, vector<double>(1) );
  random_matrix (other, 10.0);

  double log_abs_det_A = MEAL::GaussJordan (test, other);

#if PRINT_MATRIX
  cerr << "log(abs(det(A)))=" << log_abs_det_A << endl;
#endif

  double tolerance = 1e-11;

  Matrix<dim,dim,double> matrix;

  for (unsigned irow=0; irow < dim; irow++)
  {
    for (unsigned icol=0; icol < dim; icol++)
    {
#if PRINT_MATRIX
      cerr << copy[irow][icol] << "\t";
#endif

      double sum = 0.0;
      for (unsigned k=0; k < dim; k++)
        sum += test[irow][k] * copy[k][icol];

      double expect = (irow == icol) ? 1.0 : 0.0;
      if ( fabs(sum - expect) > tolerance)
      {
        cerr << "irow=" << irow << " icol=" << icol << " sum=" << sum << endl;
        return -1;
      }
    }

#if PRINT_MATRIX
    cerr << endl;
#endif
  }

  for (unsigned irow=0; irow < dim; irow++)
    for (unsigned icol=0; icol <= irow; icol++)
    {
      // construct a symmetric matric from the original test matrix
      matrix[irow][icol] = matrix[icol][irow] = copy[irow][icol];
      test[irow][icol] = test[icol][irow] = copy[irow][icol];
    }

  log_abs_det_A = MEAL::GaussJordan (test, other);

  // estimate the determinant via the product of eigenvalues
  Matrix<dim,dim,double> evec;
  Vector<dim,double> eval;
  Jacobi (matrix, evec, eval);

  double det_Jacobi = 0.0;
  for (unsigned irow=0; irow < dim; irow++)
    det_Jacobi += log(fabs(eval[irow]));

  if (fabs(log_abs_det_A - det_Jacobi) > tolerance)
  {
    cerr << "log(determinant) (via GaussJordan) = " << log_abs_det_A << endl
         << "log(determinant) (via Jacobi) = " << det_Jacobi << endl;
    return -1;
  }

  return 0;
}

int main ()
{
  unsigned ntrial = 1000;
  unsigned nerr = 0;

  cerr << "performing Gauss-Jordan elimination on " << ntrial << " " << dim << "x" << dim << " matrices" << endl;
  for (unsigned i=0; i < ntrial; i++) try
  {
    if (once () != 0)
      nerr ++;
  }
  catch (Error& error)
  {
    cerr << error << endl;
    nerr ++;
  }

  cerr << nerr << " errors in " << ntrial << " trials" << endl;
  return 0;
}

