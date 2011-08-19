/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/JonesSpinor.h"
#include <assert.h>

using namespace std;

#define POLE_UNSET 2

MEAL::JonesSpinor::JonesSpinor ()
{
  pole = POLE_UNSET;
}

std::string MEAL::JonesSpinor::get_name () const
{
  return "JonesSpinor";
}

MEAL::Spinor::Result spinor (const Jones<double>& J, unsigned pole)
{
  MEAL::Spinor::Result result;

  double mode = J(pole,pole).real();
  if (mode <= 0.0)
    return result;

  double norm = sqrt( mode );
  result[pole] = norm;
  result[!pole] = J(!pole,pole)/norm;

  return result;
}

MEAL::Spinor::Result spinor (const Jones<double>& J, 
			     const Jones<double>& dJ,
			     unsigned pole)
{
  MEAL::Spinor::Result result;

  double mode = J(pole,pole).real();
  if (mode <= 0.0)
    return result;

  double norm = sqrt( mode );
  result[pole] = 0.5 * dJ(pole,pole)/norm;
  result[!pole] = dJ(!pole,pole)/norm - J(!pole,pole)*result[pole]/(norm*norm);

  return result;
}

unsigned get_pole (const Jones<double>& J)
{
  if (J(0,0).real() > J(1,1).real())
    return 0;
  else
    return 1;
}


// defined in SpinorJones.C
Jones<double> outer (Vector<2, complex<double> >& A,
		     Vector<2, complex<double> >& B);

//! Calculate the Mueller matrix and its gradient
void MEAL::JonesSpinor::calculate (Spinor::Result& result,
				   std::vector< Spinor::Result >* grad)
{
  std::vector< Jones<double> > jones_grad;
  std::vector< Jones<double> >* jones_grad_ptr = 0;
  
  if (grad)
    jones_grad_ptr = &jones_grad;

  Jones<double> jones_result = get_model()->evaluate (jones_grad_ptr);

  if (pole == POLE_UNSET)
    pole = get_pole (jones_result);

  if (verbose)
    cerr << "MEAL::JonesSpinor::calculate pole=" << pole << endl;

  result = spinor( jones_result, pole );

  if (verbose)
    cerr << "J=  " << jones_result << endl
	 << "S*S=" << outer(result,result) << endl;

  if (!grad)
    return;

  assert( grad->size() == jones_grad.size() );

  for (unsigned i=0; i<grad->size(); i++)
  {
    (*grad)[i] = spinor( jones_result, jones_grad[i], pole );
    
    if (!verbose)
      continue;

    cerr << "spinor gradient[" << i << "]=" << (*grad)[i] << endl;

    if (i==0)
    {
      // dSpinor_dQ = (P+Q, -U-iV) / 2P*sqrt(P+Q)
      Spinor::Result dSpinor_dQ;
      dSpinor_dQ[pole]  = jones_result(pole,pole);
      dSpinor_dQ[!pole] = -jones_result(!pole,pole);

      double P = trace(jones_result).real();
      dSpinor_dQ /= 2*P*result[pole];

      cerr << "expected gradient=" << dSpinor_dQ << endl;

      cerr << "Egrad[0]=" 
	   << outer(result,dSpinor_dQ) + outer(dSpinor_dQ,result) << endl;
    }

    cerr << "Jgrad[" << i << "]=" << jones_grad[i] << endl;
    cerr << "Sgrad[" << i << "]=" 
	 << outer(result,(*grad)[i]) + outer((*grad)[i],result) << endl;
  }

}

