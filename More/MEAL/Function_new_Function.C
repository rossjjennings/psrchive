#include "MEPL/Agent.h"
#include "MEPL/Function.h"

Registry::List<Model::Agent> Model::Agent::registry;

#include "MEPL/Polynomial.h"
static Model::Advocate<Model::Polynomial> polynomial;

#include "MEPL/Gaussian.h"
static Model::Advocate<Model::Gaussian> gaussian;

#include "MEPL/SumRule.h"
typedef Model::SumRule<Model::Scalar> ScalarSum;
static Model::Advocate<ScalarSum> scalar_sum;

//! Construct a new model instance from a string
Model::Function* Model::Function::new_Function (const string& text)
{
  for (unsigned agent=0; agent<Agent::registry.size(); agent++) {

    if (Agent::registry[agent]->get_name () == text) {

      if (verbose)
        cerr << "Model::Function::new_Function using " 
	     << Agent::registry[agent]->get_name() << endl;

      return Agent::registry[agent]->new_Function();
      
    }

  }

  throw Error (InvalidParam, "Model::Function::new_Function",
	       "no match for '%s' in %d agents", text.c_str(),
	       Agent::registry.size());

}
