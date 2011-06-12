/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/SpinorJones.h"
#include "MEAL/ProjectGradient.h"

#include <assert.h>

using namespace std;

MEAL::SpinorJones::SpinorJones () : composite (this)
{
}

std::string MEAL::SpinorJones::get_name () const
{
  return "SpinorJones";
}

void MEAL::SpinorJones::set_spinorA (Spinor* A)
{
  set_spinor( spinorA, A );
}

void MEAL::SpinorJones::set_spinorB (Spinor* B)
{
  set_spinor( spinorB, B );
}

/*! unmaps the old spinor before mapping xform */
void
MEAL::SpinorJones::set_spinor (Project<Spinor>& spinor, Spinor* _spinor) try
{
  if (!_spinor)
    return;

  if (spinor)
  {
    if (Complex2::verbose)
      std::cerr << "MEAL::SpinorJones::set_spinor unmap old" << std::endl;

    composite.unmap (spinor);
  }

  spinor = _spinor;

  if (Complex2::verbose)
    std::cerr << "MEAL::SpinorJones::set_spinor map new" << std::endl;

  composite.map (spinor);
}
catch (Error& error)
{
  throw error += "MEAL::SpinorJones::set_spinor";
}

Jones<double> outer (Vector<2, complex<double> >& A,
		     Vector<2, complex<double> >& B)
{
  Jones<double> result;
  for (unsigned i=0; i<2; i++)
    for (unsigned j=0; j<2; j++)
      result(i,j) = A[i] * conj(B[j]);

  return result;
}

//! Calculate the Mueller matrix and its gradient
void MEAL::SpinorJones::calculate (Jones<double>& result,
				   std::vector< Jones<double> >* grad)
{
  std::vector< Vector<2, complex<double> > > Agrad;
  std::vector< Vector<2, complex<double> > >* Agrad_ptr = &Agrad;

  std::vector< Vector<2, complex<double> > > Bgrad;
  std::vector< Vector<2, complex<double> > >* Bgrad_ptr = &Bgrad;
  
  if (!grad)
    Agrad_ptr = Bgrad_ptr = NULL;

  Vector<2,complex<double> > Aresult = spinorA->evaluate (Agrad_ptr);
  Vector<2,complex<double> > Bresult = spinorB->evaluate (Bgrad_ptr);

  result = outer( Aresult, Bresult );

  if (!grad)
    return;

  // resize the gradient for the partial derivatives wrt all parameters
  grad->resize (get_nparam());
  for (unsigned igrad = 0; igrad<grad->size(); igrad++)
    (*grad)[igrad] = 0;

  vector< Jones<double> > temp;

  // compute the partial derivatives wrt spinorA parameters
  temp.resize( Agrad.size() );
  for (unsigned igrad = 0; igrad<Agrad.size(); igrad++)
    temp[igrad] = outer( Agrad[igrad], Bresult );

  // map the spinorA parameter gradient elements
  ProjectGradient (spinorA, temp, *grad);

  // compute the partial derivatives wrt spinorB parameters
  temp.resize( Bgrad.size() );
  for (unsigned igrad = 0; igrad<Bgrad.size(); igrad++)
    temp[igrad] = outer( Aresult, Bgrad[igrad] );

  // map the spinorB parameter gradient elements
  ProjectGradient (spinorB, temp, *grad);

}

