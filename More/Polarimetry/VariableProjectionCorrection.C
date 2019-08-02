/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/VariableProjectionCorrection.h"
#include "Pulsar/Integration.h"
#include "Pulsar/ProjectionCorrection.h"

using namespace Pulsar;

Jones<double> VariableProjectionCorrection::get_transformation ()
{
  // use the ProjectionCorrection class to calculate the transformation
  correction.set_archive (archive);
  description = correction.get_summary();
  return correction (subint);
}
