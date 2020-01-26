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
  const Integration* the_subint = archive->get_Integration(subint);
  MJD epoch = the_subint->get_epoch();
  double freq = the_subint->get_centre_frequency (chan);

  ManualPolnCalibrator::Entry best_match = calibrator->match(epoch, freq);

  Jones<double> retval = best_match.get_response();

#if 0
  cerr << "ManualVariableTransformation::get_transformation"
       << " subint=" << subint << " data epoch=" << epoch.printdays(13) 
       << " chan=" << chan << " data freq=" << freq << endl
       << " cal epoch=" << best_match.ref_epoch.printdays(13)
       << " cal freq=" << best_match.ref_frequency*1e-6 
       << " det(J)=" << det(retval) << endl;
#endif

  return retval;
}

