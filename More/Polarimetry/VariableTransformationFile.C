/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/VariableTransformationFile.h"
#include "Pulsar/Integration.h"

using namespace Pulsar;
using namespace std;

VariableTransformationFile::VariableTransformationFile (ManualPolnCalibrator* _calibrator)
{
  calibrator = _calibrator;
}

LabelledJones<double> VariableTransformationFile::get_value ()
{
  const Integration* the_subint = archive->get_Integration(subint);
  MJD epoch = the_subint->get_epoch();
  double freq = the_subint->get_centre_frequency (chan);

  auto& freq_response = calibrator->match(epoch);
  auto& best_match = freq_response.match(freq);

  LabelledJones<double> retval = best_match.get_response();
  retval.label = "from file";
  
  if (Archive::verbose > 1)
  {
    cerr << "VariableTransformationFile::get_value"
        << " subint=" << subint << " data epoch=" << epoch.printdays(13) 
        << " chan=" << chan << " data freq=" << freq << endl
        << " cal epoch=" << freq_response.get_epoch().printdays(13)
        << " cal freq=" << best_match.get_frequency()*1e-6 
        << " det(J)=" << det(retval) << endl;
  }

  if (Archive::verbose > 2)
    cerr << "VariableTransformationFile::get_value chan=" << chan << " result=" << retval << endl;
  
  return retval;
}

