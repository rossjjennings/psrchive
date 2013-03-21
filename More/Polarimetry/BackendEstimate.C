/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/BackendEstimate.h"
#include "Pulsar/SingleAxis.h"

void Calibration::BackendEstimate::update ()
{
  SingleAxis* single = dynamic_cast<SingleAxis*>( backend.get() );
  if (single)
    estimate.update (single);
}
