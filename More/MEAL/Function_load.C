#include "MEPL/Function.h"
#include "MEPL/Factory.h"

Model::Function* Model::Function::load (const string& filename) try {

  Functor< Function*(string) > constructor( &new_Function );

  return Factory::load (filename, constructor, verbose);

}
catch (Error& error) {
  throw error += "Model::Function::load";
}

