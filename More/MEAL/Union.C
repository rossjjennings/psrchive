/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Union.h"
#include "MEAL/ProjectGradient.h"

using namespace std;

MEAL::Union::Union (Composite* policy)
{
  if (policy)
    composite = policy;
  else
    composite = new Composite (this);

  dimension = 0;
}

//! Copy constructor
MEAL::Union::Union (const Union& that)
{
  composite = new Composite (this);
  dimension = 0;
  operator = (that);
}

MEAL::Union& MEAL::Union::operator = (const Union& copy)
{
  if (this == &copy)
    return *this;

  unsigned nmodel = copy.model.size();
  for (unsigned imodel=0; imodel < nmodel; imodel++)
    push_back (copy.model[imodel]);

  ScalarVector::operator = (copy);

  return *this;
}


void MEAL::Union::push_back (ScalarVector* subspace)
{
  if (very_verbose)
    std::cerr << "MEAL::Union::push_back" << std::endl;

  dimension += subspace->size();
  model.push_back (Project<ScalarVector>(subspace));
  composite->map (model.back());
}

void MEAL::Union::calculate (double& result, std::vector<double>* grad)
{
  unsigned nmodel = model.size();

  if (very_verbose)
    std::cerr << "MEAL::Union::calculate nmodel=" << nmodel << std::endl;

  // the gradient of each component
  std::vector<double> comp_gradient;

  // the pointer to the above array, if grad != 0
  std::vector<double>* comp_gradient_ptr = &comp_gradient;
  if (!grad)
    comp_gradient_ptr = 0;

  unsigned model_index = 0;
  unsigned remaining_index = get_index();

  if (very_verbose)
    cerr << "MEAL::Union::calculate index=" << remaining_index << endl;

  while (model_index < model.size())
  {
    if (remaining_index < model[model_index]->size())
      break;
    else
      remaining_index -= model[model_index]->size();

    model_index ++;
  }

  if (model_index == model.size())
    throw Error (InvalidState, "MEAL::Union::calculate",
		 "index=%u out of range", get_index());

  if (get_verbose())
    std::cerr << "MEAL::Union::calculate evaluate " 
	      << model[model_index]->get_name() << std::endl;

  try {

    model[model_index]->set_index( remaining_index );

    // evaluate the model and its gradient
    result = model[model_index]->evaluate (comp_gradient_ptr);

    if (get_verbose())
      std::cerr << "MEAL::Union::calculate " 
		<< model[model_index]->get_name()
		<< " result=" << result << std::endl;

  }
  catch (Error& error)
  {
    error += "MEAL::Union::calculate";
    throw error << " model=" << model[model_index]->get_name();
  }
  
  if (grad)
  {
    if (model[model_index]->get_nparam() != comp_gradient.size())
      throw Error (InvalidState, "MEAL::Union::calculate",
		   "model[%d]=%s.get_nparam=%d != gradient.size=%d",
		   model_index, model[model_index]->get_name().c_str(),
		   model[model_index]->get_nparam(), comp_gradient.size());
    
    unsigned nparam = get_nparam();

    grad->resize (nparam);
    for (unsigned iparam=0; iparam<nparam; iparam++)
      (*grad)[iparam] = 0.0;

    if (get_verbose())
      cerr << "MEAL::Union::calculate project gradient" << endl;

    // re-map the elements of the component gradient into the Composite space
    ProjectGradient (model[model_index], comp_gradient, *grad);

    if (get_verbose())
      cerr << "MEAL::Union::calculate gradient projected" << endl;
  }

  if (very_verbose)
  {
    std::cerr << "MEAL::Union::calculate model index=" << model_index
	      << " result\n   " << result << std::endl;
    if (grad)
    {
      std::cerr << "MEAL::Union::calculate gradient" << std::endl;
      for (unsigned i=0; i<grad->size(); i++)
	std::cerr << "   " << i << ":" << get_infit(i) 
		  << "=" << (*grad)[i] << std::endl;
    }
  }
}
