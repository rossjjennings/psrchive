#include "MEAL/Function.h"
#include "MEAL/Factory.h"

MEAL::Function* MEAL::Function::load (const std::string& filename) try {

  Functor< Function*(std::string) > constructor( &new_Function );

  return Factory::load (filename, constructor, verbose);

}
catch (Error& error) {
  throw error += "MEAL::Function::load";
}

