//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/InverseRule.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:15 $
   $Author: straten $ */

#ifndef __Calibration_InverseRule_H
#define __Calibration_InverseRule_H

#include "Calibration/UnaryRule.h"

namespace Calibration {

  //! Computes the inverse of a model and its partial derivatives
  template<class MType>
  class InverseRule : public UnaryRule<MType>
  {

  public:

    typedef typename MType::Result Result;

    // ///////////////////////////////////////////////////////////////////
    //
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    string get_name () const;

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // OptimizedModel implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the Result and its gradient
    void calculate (Result& result, vector<Result>* gradient);

  };

}

template<class MType>
string Calibration::InverseRule<MType>::get_name () const
{
  return "InverseRule<" + string(MType::Name)+ ">";
}


template<class MType>
void Calibration::InverseRule<MType>::calculate (Result& result,
						 vector<Result>* grad)
{
  if (!this->model)
    throw Error (InvalidState, "Calibration::InverseRule::calculate",
		 "no model to evaluate");

  if (MType::verbose)
    cerr << "Calibration::InverseRule::calculate" << endl;

  result = inv( this->model->evaluate (grad) );
  
  if (MType::verbose)
    cerr << "Calibration::InverseRule::calculate result\n"
      "   " << result << endl;

  if (!grad)
    return;

  for (unsigned igrad=0; igrad<grad->size(); igrad++)
    (*grad)[igrad] = - result * (*grad)[igrad] * result;

  if (MType::verbose) {
    cerr << "Calibration::InverseRule::calculate gradient\n";
    for (unsigned i=0; i<grad->size(); i++)
      cerr << "   "
	   << i << ":" << this->get_infit(i) << "=" << (*grad)[i] << endl;
  }

}


#endif
