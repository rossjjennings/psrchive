/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ManualVariableTransformation.h"
#include "Pulsar/Integration.h"

using namespace Pulsar;

ManualVariableTransformation::ManualVariableTransformation (ManualPolnCalibrator* _calibrator)
{
  calibrator = _calibrator;
}

Jones<double> ManualVariableTransformation::get_transformation ()
{
  const Integration* the_subint = archive->get_Integration(subint);
  MJD epoch = the_subint->get_epoch();
  double freq = the_subint->get_centre_frequency (chan);

  ManualPolnCalibrator::Entry best_match = calibrator->match(epoch, freq);

  return best_match.get_response();
}
