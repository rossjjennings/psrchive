//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/GroupRule.h,v $
   $Revision: 1.2 $
   $Date: 2004/11/22 16:00:09 $
   $Author: straten $ */

#ifndef __GroupRule_H
#define __GroupRule_H

#include "MEPL/ProjectGradient.h"
#include "MEPL/Optimized.h"
#include "MEPL/Composite.h"
#include "stringtok.h"

namespace Model {

  //! Abstract base class of closed, associative, binary operators
  /*! Because the binary operation is associative, this class is
    implemented as a series of elements; that is, an arbitrary
    number of models can be added.  By inheriting
    this class and defining the initialize() and operate() pure
    virtual methods, derived classes may define the closed, associative
    binary operation, such as the product or sum. */

  template<class MType>
  class GroupRule : public Optimized<MType>, public Composite 
  {

  public:

    typedef typename MType::Result Result;

    //! Default constructor
    GroupRule () { }

    //! Copy constructor
    GroupRule (const GroupRule& meta) { operator = (meta); }

    //! Assignment operator
    GroupRule& operator = (const GroupRule& meta);

    //! Destructor
    ~GroupRule () { }

    //! Add an element to the result
    void add_model (MType* model);

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Parse the values of model parameters and fit flags from a string
    void parse (const string& text);

    // ///////////////////////////////////////////////////////////////////
    //
    // Optimized template implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the result and its gradient
    void calculate (Result& result, vector<Result>* grad);

    // ///////////////////////////////////////////////////////////////////
    //
    // Composite template implementation
    //
    // ///////////////////////////////////////////////////////////////////

    string class_name() const
    { return "Model::GroupRule[" + get_name() + "]::"; }

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Prints the values of model parameters and fit flags to a string
    void print_parameters (string& text, const string& sep) const;

    //! Return the group identity
    virtual const Result get_identity () const = 0;

    //! Multiply the total by the element
    virtual void operate (Result& total, const Result& element) = 0;

    //! Neighbouring terms stay in each other's partial derivatives
    virtual const Result partial (const Result& element) const = 0;

  private:

    //! The models and their mappings
    vector< Project<MType> > model;

    //! The result
    Result result;

    //! The gradient
    vector<Result> gradient;

    //! Initialize the result and gradient attributes
    void initialize ();

  };
  
}


template<class MType>
Model::GroupRule<MType>&
Model::GroupRule<MType>:: operator = (const GroupRule& meta)
{
  if (this == &meta)
    return *this;

  unsigned nmodel = meta.model.size();
  for (unsigned imodel=0; imodel < nmodel; imodel++)
    add_model (meta.model[imodel]);

  // name = meta.name;

  return *this;
}

template<class MType>
void Model::GroupRule<MType>::print_parameters (string& text,
                                                      const string& sep) const
{
  unsigned nmodel = model.size();
  for (unsigned imodel=0; imodel < nmodel; imodel++) {
    text += sep + model[imodel]->get_name ();
    model[imodel]->print_parameters (text, sep + " ");
  }
}

template<class MType>
void Model::GroupRule<MType>::parse (const string& line)
{
  if (model.size()) try {
    model.back()->parse(line);
    return;
  }
  catch (Error& error) {
  }

  // the key should be the name of a new class to be added
  string temp = line;
  string key = stringtok (temp, " \t");

  if (verbose)
    cerr << class_name() << "::parse key '" << key << "'" << endl;

  Function* model = Function::new_Function (key);

  MType* mtype = dynamic_cast<MType*>(model);
  if (!mtype)
    throw Error (InvalidParam, get_name()+"::parse",
		 model->get_name() + " is not of type " + string(MType::Name));

  add_model (mtype);
}

template<class MType>
void Model::GroupRule<MType>::add_model (MType* x)
{
  if (very_verbose)
    cerr << class_name() + "add_model" << endl;

  model.push_back (Project<MType>(x));
  map (model.back());
}

template<class MType>
void Model::GroupRule<MType>::initialize ()
{
  result = get_identity();

  for (unsigned jgrad=0; jgrad<gradient.size(); jgrad++)
    gradient[jgrad] = get_identity();
}

template<class MType>
void Model::GroupRule<MType>::calculate (Result& retval,
					       vector<Result>* grad)
{
  unsigned nmodel = model.size();

  if (very_verbose)
    cerr << class_name() + "calculate nmodel=" << nmodel << endl;

  // the result of each component
  Result comp_result;

  // the gradient of each component
  vector<Result> comp_gradient;

  // the pointer to the above array, if grad != 0
  vector<Result>* comp_gradient_ptr = 0;
  
  unsigned total_nparam = 0;

  if (grad) {

    for (unsigned imodel=0; imodel < nmodel; imodel++)
      total_nparam += model[imodel]->get_nparam();

    comp_gradient_ptr = &comp_gradient;
    gradient.resize (total_nparam);

  }

  // initialize the result and gradient attributes
  initialize ();

  unsigned igradient = 0;

  for (unsigned imodel=0; imodel < nmodel; imodel++) {

    if (very_verbose) cerr << class_name() + "calculate evaluate " 
			   << model[imodel]->get_name() << endl;

    try {

      // evaluate the model and its gradient
      comp_result = model[imodel]->evaluate (comp_gradient_ptr);

      operate( result, comp_result );
      
      if (grad) {

	unsigned jgrad;
	unsigned ngrad = comp_gradient_ptr->size();

	for (jgrad=0; jgrad<igradient; jgrad++)
	  operate( gradient[jgrad], partial(comp_result) );
	
	for (jgrad=0; jgrad<ngrad; jgrad++)
	  operate( gradient[igradient + jgrad], (*comp_gradient_ptr)[jgrad] );
	
	for (jgrad=igradient+ngrad; jgrad<gradient.size(); jgrad++)
	  operate( gradient[jgrad], partial(comp_result) );
	
      }

    }
    catch (Error& error) {
      error += class_name() + "calculate";
      throw error << " model=" << model[imodel]->get_name();
    }

    if (grad) {

      if (model[imodel]->get_nparam() != comp_gradient.size())
	throw Error (InvalidState, (class_name() + "calculate").c_str(),
		     "model[%d]=%s.get_nparam=%d != gradient.size=%d",
		     imodel, model[imodel]->get_name().c_str(),
		     model[imodel]->get_nparam(), comp_gradient.size());
      
      igradient += comp_gradient.size();

    }

  }

  retval = result;

  if (grad)  {

    /* re-map the components of the gradient into the Composite space,
       summing duplicates implements both the sum and product rules. */

    // sanity check, ensure that all elements have been set
    if (igradient != total_nparam)
      throw Error (InvalidState, (class_name() + "calculate").c_str(),
		   "after calculation igrad=%d != total_nparam=%d",
		   igradient, total_nparam);

    grad->resize (get_nparam());

    // this verion of ProjectGradient initializes the gradient vector to zero
    ProjectGradient (model, gradient, *grad);

  }

  if (very_verbose) {
    cerr << class_name() + "calculate result\n   " << retval << endl;
    if (grad) {
      cerr << class_name() + "calculate gradient" << endl;
      for (unsigned i=0; i<grad->size(); i++)
	cerr << "   " << i << ":" << get_infit(i) << "=" << (*grad)[i] << endl;
    }
  }
}

#endif

