#include "MEAL/Agent.h"
#include "MEAL/Function.h"

Registry::List<MEAL::Agent> MEAL::Agent::registry;

#include "MEAL/Polynomial.h"
static MEAL::Advocate<MEAL::Polynomial> polynomial;

#include "MEAL/Gaussian.h"
static MEAL::Advocate<MEAL::Gaussian> gaussian;

#include "MEAL/SumRule.h"
typedef MEAL::SumRule<MEAL::Scalar> ScalarSum;
static MEAL::Advocate<ScalarSum> scalar_sum;

//! Construct a new model instance from a string
MEAL::Function* MEAL::Function::new_Function (const std::string& text)
{
  for (unsigned agent=0; agent<Agent::registry.size(); agent++) {

    if (Agent::registry[agent]->get_name () == text) {

      if (verbose)
        cerr << "MEAL::Function::new_Function using " 
	     << Agent::registry[agent]->get_name() << endl;

      return Agent::registry[agent]->new_Function();
      
    }

  }

  throw Error (InvalidParam, "MEAL::Function::new_Function",
	       "no match for '%s' in %d agents", text.c_str(),
	       Agent::registry.size());

}
