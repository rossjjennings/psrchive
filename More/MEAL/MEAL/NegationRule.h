//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/NegationRule.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:16 $
   $Author: straten $ */

#ifndef __Calibration_NegationRule_H
#define __Calibration_NegationRule_H

#include "Calibration/UnaryRule.h"

namespace Calibration {

  //! Computes the inverse of a model and its partial derivatives
  template<class MType>
  class NegationRule : public UnaryRule<MType>
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
string Calibration::NegationRule<MType>::get_name () const
{
  return "NegationRule<" + string(MType::Name)+ ">";
}


template<class MType>
void Calibration::NegationRule<MType>::calculate (Result& result,
						  vector<Result>* grad)
{
  if (!this->model)
    throw Error (InvalidState, "Calibration::NegationRule::calculate",
		 "no model to evaluate");

  if (MType::verbose)
    cerr << "Calibration::NegationRule::calculate" << endl;

  result = - this->model->evaluate (grad);
  
  if (MType::verbose)
    cerr << "Calibration::NegationRule::calculate result\n"
      "   " << result << endl;

  if (!grad)
    return;

  for (unsigned igrad=0; igrad<grad->size(); igrad++)
    (*grad)[igrad] *= -1.0;

  if (MType::verbose) {
    cerr << "Calibration::NegationRule::calculate gradient\n";
    for (unsigned i=0; i<grad->size(); i++)
      cerr << "   "
	   << i << ":" << this->get_infit(i) << "=" << (*grad)[i] << endl;
  }

}


#endif
