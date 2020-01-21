/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ManualVariableTransformation.h"
#include "Pulsar/Integration.h"

using namespace Pulsar;
using namespace std;

ManualVariableTransformation::ManualVariableTransformation (ManualPolnCalibrator* _calibrator)
{
  calibrator = _calibrator;
}

Jones<double> ManualVariableTransformation::get_transformation ()
{
  MJD epoch = archive->get_Integration(subint)->get_epoch();

  std::vector<ManualPolnCalibrator::Entry> best_match;
  best_match = calibrator->match(epoch);

  Jones<double> retval = best_match.at(chan).get_response();

  cerr << "ManualVariableTransformation::get_transformation epoch=" << epoch.printdays(13) << " chan=" << chan
       << " data freq=" << archive->get_Integration(subint)->get_centre_frequency(chan) 
       << " cal freq=" << best_match.at(chan).ref_frequency*1e-6 << " response.size=" << best_match.size()
       << " det(J)=" << det(retval) << endl;

  return retval;
}
