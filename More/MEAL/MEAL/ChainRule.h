//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ChainRule.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 19:26:03 $
   $Author: straten $ */

#ifndef __MEAL_ChainRule_H
#define __MEAL_ChainRule_H

#include "MEAL/ProjectGradient.h"
#include "MEAL/Optimized.h"
#include "MEAL/Composite.h"
#include "MEAL/Scalar.h"

namespace MEAL {

  //! A parameter and its constraining Scalar instance
  class ConstrainedParameter {

  public:
    ConstrainedParameter (unsigned iparam, Scalar* func)
      : scalar (func)
    {
      parameter = iparam;
    }

    //! The index of the Function parameter constrained by scalar
    unsigned parameter;

    //! The constraining Scalar instance
    Project<Scalar> scalar;

    //! The last calculated gradient of the Scalar
    std::vector<double> gradient;

  };

  //! Parameterizes a Function by one or more Scalar ordinates.

  /*! Given any Function, \f$ M({\bf a}) \f$, one or more parameters may
    be constrained by a Scalar, \f$ f({\bf b}) \f$.  That is,
    \f$a_i=f({\bf b})\f$.  The fit flag for \f$a_i\f$ is set to false,
    and the partial derivatives of \f$ M \f$ with respect to the
    Scalar parameters, \f$\bf b\f$, are given by the chain rule,
    \f${\partial M\over\partial b_k} = {\partial M\over\partial
    a_i}{\partial f\over\partial b_k}\f$. */

  template<class MType>
  class ChainRule : public Optimized<MType>, public Composite
  {

  public:

    typedef typename MType::Result Result;

    //! Default constructor
    ChainRule () { }

    //! Copy constructor
    ChainRule (const ChainRule& rule) { operator = (rule); }

    //! Assignment operator
    ChainRule& operator = (const ChainRule& rule);

    //! Set the Function to be constrained by Scalar ordinates
    void set_model (MType* model);

    //! Set the Scalar instance used to constrain the specified parameter
    void set_constraint (unsigned iparam, Scalar* scalar);

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

    //! Scalars used to constrain the parameters
    std::vector<ConstrainedParameter> constraints;

    //! The Function to be constrained by Scalar ordinates
    Project<MType> model;

  };

}

template<class MType>
std::string MEAL::ChainRule<MType>::get_name () const
{
  return "ChainRule<" + std::string(MType::Name)+ ">";
}

template<class MType>
MEAL::ChainRule<MType>&
MEAL::ChainRule<MType>::operator = (const ChainRule& rule)
{
  if (this == &rule)
    return *this;

  set_model (rule.model);

  for (unsigned ic=0; ic < rule.constraints.size(); ic++)
    set_constraint (rule.constraints[ic].parameter,
		    rule.constraints[ic].scalar);

  return *this;
}


template<class MType>
void MEAL::ChainRule<MType>::set_constraint (unsigned iparam, 
						    Scalar* scalar)
{
  if (verbose) 
    std::cerr << "MEAL::ChainRule::set_constraint iparam=" << iparam << std::endl;

  if (!scalar)
    return;

  // only one scalar may control a parameter
  for (unsigned ifunc=0; ifunc<constraints.size(); ifunc++) {

    if (constraints[ifunc].parameter == iparam) {

      if (verbose)
	std::cerr << "MEAL::ChainRule::set_constraint"
	  " replace param=" << iparam << std::endl;

      unmap (constraints[ifunc].scalar, false);
      constraints[ifunc].scalar = scalar;
      map (constraints[ifunc].scalar);

      return;

    }
  }

  constraints.push_back (ConstrainedParameter (iparam, scalar));
  map (constraints.back().scalar);

  if (model)
    model->set_infit (iparam, false);

}

template<class MType>
void MEAL::ChainRule<MType>::set_model (MType* _model)
{
  if (!_model)
    return;

  if (model) {
    if (verbose)
      std::cerr << "MEAL::ChainRule::set_model"
	" unmap old model" << std::endl;
    unmap (model, false);
  }

  model = _model;

  if (verbose)
    std::cerr << "MEAL::ChainRule::set_model"
      " map new model" << std::endl;

  map (model);

  for (unsigned ifunc=0; ifunc<constraints.size(); ifunc++)
    model->set_infit (constraints[ifunc].parameter, false);
}

template<class MType>
void MEAL::ChainRule<MType>::calculate (Result& result,
					       std::vector<Result>* grad)
{
  if (!model)
    throw Error (InvalidState, "MEAL::ChainRule::calculate","no model");

  if (verbose)
    std::cerr << "MEAL::ChainRule::calculate" << std::endl;

  for (unsigned ifunc=0; ifunc<constraints.size(); ifunc++) {

    if (very_verbose)
      std::cerr << "MEAL::ChainRule::calculate iconstraint="<< ifunc <<std::endl;

    std::vector<double>* fgrad = 0;
    if (grad)
      fgrad = &(constraints[ifunc].gradient);

    model->set_param (constraints[ifunc].parameter,
		      constraints[ifunc].scalar->evaluate(fgrad));

  }

  std::vector<Result> model_grad;
  std::vector<Result>* model_grad_ptr = 0;
  if (grad)
    model_grad_ptr = & model_grad;

  result = model->evaluate (model_grad_ptr);
  
  if (grad) {

    unsigned ngrad = get_nparam();
    grad->resize (ngrad);

    unsigned igrad;
    for (igrad=0; igrad<ngrad; igrad++)
      (*grad)[igrad] = 0.0;

    // map the model gradient
    ProjectGradient (model, model_grad, *(grad));

    // map the scalar gradients
    std::vector<Result> fgrad;

    for (unsigned ifunc=0; ifunc<constraints.size(); ifunc++) {

      unsigned iparam = constraints[ifunc].parameter;

      ngrad = constraints[ifunc].gradient.size();
      fgrad.resize (ngrad);

      // dM/dxi = dM/df * df/dxi, where M=model, f=func, xi=func.param[i]
      for (igrad=0; igrad<ngrad; igrad++)
	fgrad[igrad] = model_grad[iparam] * constraints[ifunc].gradient[igrad];


      ProjectGradient (constraints[ifunc].scalar, fgrad, *(grad));

    }
  
  }

  if (verbose) {
    std::cerr << "MEAL::ChainRule::calculate result\n"
      "   " << result << std::endl;
    if (grad) {
      std::cerr << "MEAL::ChainRule::calculate gradient\n";
      for (unsigned i=0; i<grad->size(); i++)
	std::cerr << "   " << i << ":" << get_infit(i) << "=" << (*grad)[i] << std::endl;
    }
  }

}

#endif
