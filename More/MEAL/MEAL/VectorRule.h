//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/VectorRule.h,v $
   $Revision: 1.2 $
   $Date: 2005/04/20 07:04:33 $
   $Author: straten $ */

#ifndef __MEAL_VectorRule_H
#define __MEAL_VectorRule_H

#include "MEAL/ProjectGradient.h"
#include "MEAL/Composite.h"
#include "stringtok.h"

namespace MEAL {

  //! VectorRule implements a vector of Functions with a current index
  template<class T>
  class VectorRule : public T
  {

  public:

    typedef typename T::Result Result;

    //! Default constructor
    VectorRule () : composite(this) { model_index = 0; }

    //! Copy constructor
    VectorRule (const VectorRule& v) : composite(this) { operator = (v); }

    //! Assignment operator
    VectorRule& operator = (const VectorRule& copy);

    //! Destructor
    ~VectorRule () { }

    //! Add an element to the array
    void push_back (T* model);

    //! Get the size of the array
    unsigned size () const { return model.size(); }

    //! Set the index of the array
    void set_index (unsigned index);

    //! Get the index of the array
    unsigned get_index () const { return model_index; }

    //! Return the name of the class
    std::string get_name () const
    { return "VectorRule<" + std::string(T::Name)+ ">"; }

  protected:

    //! Return the result and its gradient
    void calculate (Result& result, std::vector<Result>* grad);

  private:

    //! The models and their mappings
    std::vector< Project<T> > model;

    //! The current index in the array
    unsigned model_index;

    //! Composite parameter policy
    Composite composite;

  };
  
}


template<class T>
MEAL::VectorRule<T>&
MEAL::VectorRule<T>:: operator = (const VectorRule& copy)
{
  if (this == &copy)
    return *this;

  unsigned nmodel = copy.model.size();
  for (unsigned imodel=0; imodel < nmodel; imodel++)
    push_back (copy.model[imodel]);

  model_index = copy.model_index;

  return *this;
}

template<class T>
void MEAL::VectorRule<T>::push_back (T* x)
{
  if (very_verbose)
    std::cerr << get_name() + "push_back" << std::endl;

  model.push_back (Project<T>(x));
  composite.map (model.back());
}

template<class T>
void MEAL::VectorRule<T>::set_index (unsigned index)
{
  if (index == model_index)
    return;

  if (index >= model.size())
    throw Error (InvalidRange, "MEAL::"+get_name()+"::set_index",
		 "index=%d >= nmodel=%d", index, model.size());

  model_index = index;
  this->set_evaluation_changed();
}

template<class T>
void MEAL::VectorRule<T>::calculate (Result& result,
				     std::vector<Result>* grad)
{
  unsigned nmodel = model.size();
  if (very_verbose)
    std::cerr << get_name() + "calculate nmodel=" << nmodel << std::endl;

  // the gradient of each component
  std::vector<Result> comp_gradient;

  // the pointer to the above array, if grad != 0
  std::vector<Result>* comp_gradient_ptr = 0;

  if (grad)
    comp_gradient_ptr = &comp_gradient;

  if (very_verbose) std::cerr << get_name() + "calculate evaluate " 
			      << model[model_index]->get_name() << std::endl;

  try {

    // evaluate the model and its gradient
    result = model[model_index]->evaluate (comp_gradient_ptr);

  }
  catch (Error& error) {
    error += get_name() + "calculate";
    throw error << " model=" << model[model_index]->get_name();
  }
  
  if (grad) {

    if (model[model_index]->get_nparam() != comp_gradient.size())
      throw Error (InvalidState, (get_name() + "calculate").c_str(),
		   "model[%d]=%s.get_nparam=%d != gradient.size=%d",
		   model_index, model[model_index]->get_name().c_str(),
		   model[model_index]->get_nparam(), comp_gradient.size());
    
    /* re-map the components of the gradient into the Composite space,
       summing duplicates implements both the sum and product rules. */

    unsigned nparam = get_nparam();

    grad->resize (nparam);
    // set each element of the gradient to zero
    for (unsigned iparam=0; iparam<nparam; iparam++)
      (*grad)[iparam] = 0.0;

    // this verion of ProjectGradient initializes the gradient vector to zero
    ProjectGradient (model[model_index], comp_gradient, *grad);

  }

  if (very_verbose) {

    std::cerr << get_name() + "calculate result\n   " << result << std::endl;
    if (grad) {
      std::cerr << get_name() + "calculate gradient" << std::endl;
      for (unsigned i=0; i<grad->size(); i++)
	std::cerr << "   " << i << ":" << get_infit(i) 
		  << "=" << (*grad)[i] << std::endl;
    }
  }
}

#endif

