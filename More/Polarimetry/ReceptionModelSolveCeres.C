/***************************************************************************
 *
 *   Copyright (C) 2017 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ReceptionModelSolveCeres.h"
#include "Pulsar/CoherencyMeasurementSet.h"

#include "ceres/ceres.h"

#include <iostream>
#include <assert.h>

using namespace std;
using Calibration::CoherencyMeasurementSet;

std::string Calibration::SolveCeres::get_name () const
{
  return "Ceres";
}

Calibration::SolveCeres* Calibration::SolveCeres::clone () const
{
  return new SolveCeres;
}

void Calibration::SolveCeres::fit ()
{
  if (Calibration::ReceptionModel::verbose)
    cerr << "Calibration::SolveCeres::Fit"
      " nfit=" << get_nparam_infit () <<
      " ndat=" << get_ndat_constraint () << endl;


  ceres::Problem problem;
}

