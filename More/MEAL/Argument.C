#include "Calibration/Argument.h"

bool Calibration::Argument::verbose = false;

Calibration::Argument::~Argument ()
{
#ifdef _DEBUG
  cerr << "Calibration::Argument::~Argument" << endl;
#endif
}

Calibration::Argument::Value::~Value ()
{
#ifdef _DEBUG
  cerr << "Calibration::Argument::Value::~Value" << endl;
#endif
}
