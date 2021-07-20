/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CoherencyMeasurementSet.h"

using namespace std;
using namespace Calibration;

CoherencyMeasurementSet::CoherencyMeasurementSet (unsigned index)
{
  transformation_index = index;
}

CoherencyMeasurementSet::~CoherencyMeasurementSet ()
{
}

//! Set the transformation through which the measurements are made
void 
CoherencyMeasurementSet::set_transformation_index (unsigned index)
{
  transformation_index = index;
}

//! Set the transformation through which the measurements are made
unsigned CoherencyMeasurementSet::get_transformation_index () const
{
  return transformation_index;
}

//! Add an independent variable
void CoherencyMeasurementSet::add_coordinate (MEAL::Argument::Value* x)
{
  coordinates.push_back (x);
}

void CoherencyMeasurementSet::set_coordinates () const
{
  for (unsigned ic=0; ic<coordinates.size(); ic++)
    coordinates[ic]->apply();
}

