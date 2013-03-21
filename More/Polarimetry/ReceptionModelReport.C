
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ReceptionModelReport.h"

#include <fstream>
using namespace std;

void Calibration::ReceptionModel::Report::report ()
{
  if (filename.empty())
    throw Error( InvalidState, "Calibration::ReceptionModel::Report::report",
		 "filename not set");

  ofstream os ( filename.c_str() );
  if (!os)
    throw Error( FailedSys, "Calibration::ReceptionModel::Report::report",
		 "ofstream (" + filename + ")" );
 
  report (os);
}
