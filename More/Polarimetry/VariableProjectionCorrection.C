/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/VariableProjectionCorrection.h"
#include "Pulsar/Integration.h"
#include "Pulsar/ProjectionCorrection.h"

using namespace Pulsar;

Jones<double> VariableProjectionCorrection::get_transformation ()
{
  if (!built)
    build ();

  return transformation;
}

bool VariableProjectionCorrection::required () const
{
  if (!built)
    build ();

  return correction.required (subint);
}

std::string VariableProjectionCorrection::get_description() const
{
  if (!built)
    build ();

  return description;
}

void VariableProjectionCorrection::build () const try
{
  if (built)
    return;

  // use the ProjectionCorrection class to calculate the transformation
  correction.set_archive (archive);
  transformation = correction (subint);
  description = correction.get_summary();
}
catch (Error& error)
{
  throw error += "VariableProjectionCorrection::build";
}

void VariableProjectionCorrection::set_chan (unsigned)
{
  // ignore (this override disables resetting the built flag)
}

