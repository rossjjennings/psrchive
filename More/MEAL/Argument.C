#include "MEPL/Argument.h"

bool Model::Argument::verbose = false;

Model::Argument::~Argument ()
{
#ifdef _DEBUG
  cerr << "Model::Argument::~Argument" << endl;
#endif
}

Model::Argument::Value::~Value ()
{
#ifdef _DEBUG
  cerr << "Model::Argument::Value::~Value" << endl;
#endif
}
