#include "MEAL/Argument.h"

bool MEAL::Argument::verbose = false;

MEAL::Argument::~Argument ()
{
#ifdef _DEBUG
  cerr << "MEAL::Argument::~Argument" << endl;
#endif
}

MEAL::Argument::Value::~Value ()
{
#ifdef _DEBUG
  cerr << "MEAL::Argument::Value::~Value" << endl;
#endif
}
