#include "MEAL/ArgumentPolicy.h"
#include "MEAL/Function.h"

MEAL::ArgumentPolicy::ArgumentPolicy (Function* context)
 : FunctionPolicy (context)
{
  context->argument_policy = this;
}

