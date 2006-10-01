/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/PolarCalibrator.h"
#include "Pulsar/InstrumentInfo.h"

using namespace std;

Pulsar::PolnCalibrator::Info* 
Pulsar::PolnCalibrator::Info::create (const Pulsar::PolnCalibrator* calibrator)
{
  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::Info::create type="
	 << Calibrator::Type2str ( calibrator->get_type() ) << endl;

  switch (calibrator->get_type()) {

  case Calibrator::SingleAxis:
    return new SingleAxisCalibrator::Info (calibrator);

  case Calibrator::Polar:
  case Calibrator::Hamaker:
    return new PolarCalibrator::Info (calibrator);

  case Calibrator::Britton:
    return new InstrumentInfo (calibrator);

  default:
    return new PolnCalibrator::Info (calibrator);
  }
}
