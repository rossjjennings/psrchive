/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ManualVariableTransformation.h"
#include "Pulsar/Integration.h"

using namespace Pulsar;

Jones<double> ManualVariableTransformation::get_transformation ()
{
  MJD epoch = archive->get_Integration(subint)->get_epoch();

  std::vector<ManualPolnCalibrator::Entry> best_match;
  best_match = calibrator->match(epoch);

  return best_match.at(chan).get_response();
}
