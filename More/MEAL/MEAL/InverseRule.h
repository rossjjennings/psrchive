//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/InverseRule.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 19:26:04 $
   $Author: straten $ */

#ifndef __MEAL_InverseRule_H
#define __MEAL_InverseRule_H

#include "MEAL/UnaryRule.h"

namespace MEAL {

  //! Computes the inverse of a model and its partial derivatives
  template<class MType>
  class InverseRule : public UnaryRule<MType>
  {

  public:

    typedef typename MType::Result Result;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // Optimized implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the Result and its gradient
    void calculate (Result& result, std::vector<Result>* gradient);

  };

}

template<class MType>
std::string MEAL::InverseRule<MType>::get_name () const
{
  return "InverseRule<" + std::string(MType::Name)+ ">";
}


template<class MType>
void MEAL::InverseRule<MType>::calculate (Result& result,
						 std::vector<Result>* grad)
{
  if (!this->model)
    throw Error (InvalidState, "MEAL::InverseRule::calculate",
		 "no model to evaluate");

  if (MType::verbose)
    std::cerr << "MEAL::InverseRule::calculate" << std::endl;

  result = inv( this->model->evaluate (grad) );
  
  if (MType::verbose)
    std::cerr << "MEAL::InverseRule::calculate result\n"
      "   " << result << std::endl;

  if (!grad)
    return;

  for (unsigned igrad=0; igrad<grad->size(); igrad++)
    (*grad)[igrad] = - result * (*grad)[igrad] * result;

  if (MType::verbose) {
    std::cerr << "MEAL::InverseRule::calculate gradient\n";
    for (unsigned i=0; i<grad->size(); i++)
      std::cerr << "   "
	   << i << ":" << this->get_infit(i) << "=" << (*grad)[i] << std::endl;
  }

}


#endif
