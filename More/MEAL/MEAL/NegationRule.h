//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/NegationRule.h,v $
   $Revision: 1.2 $
   $Date: 2004/11/22 16:00:09 $
   $Author: straten $ */

#ifndef __Model_NegationRule_H
#define __Model_NegationRule_H

#include "MEPL/UnaryRule.h"

namespace Model {

  //! Computes the inverse of a model and its partial derivatives
  template<class MType>
  class NegationRule : public UnaryRule<MType>
  {

  public:

    typedef typename MType::Result Result;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    string get_name () const;

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // Optimized implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the Result and its gradient
    void calculate (Result& result, vector<Result>* gradient);

  };

}

template<class MType>
string Model::NegationRule<MType>::get_name () const
{
  return "NegationRule<" + string(MType::Name)+ ">";
}


template<class MType>
void Model::NegationRule<MType>::calculate (Result& result,
						  vector<Result>* grad)
{
  if (!this->model)
    throw Error (InvalidState, "Model::NegationRule::calculate",
		 "no model to evaluate");

  if (MType::verbose)
    cerr << "Model::NegationRule::calculate" << endl;

  result = - this->model->evaluate (grad);
  
  if (MType::verbose)
    cerr << "Model::NegationRule::calculate result\n"
      "   " << result << endl;

  if (!grad)
    return;

  for (unsigned igrad=0; igrad<grad->size(); igrad++)
    (*grad)[igrad] *= -1.0;

  if (MType::verbose) {
    cerr << "Model::NegationRule::calculate gradient\n";
    for (unsigned i=0; i<grad->size(); i++)
      cerr << "   "
	   << i << ":" << this->get_infit(i) << "=" << (*grad)[i] << endl;
  }

}


#endif
