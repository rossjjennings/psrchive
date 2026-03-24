/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/VariableProjectionCorrection.h"
#include "Pulsar/Integration.h"
#include "Pulsar/ProjectionCorrection.h"

#include <iostream>

using namespace std;
using namespace Pulsar;

//! Get the feed projection
LabelledJones<double> VariableProjectionCorrection::get_feed_projection ()
{
  if (!built)
    build ();

  return feed_projection;
}

//! Get the antenna projection
LabelledJones<double> VariableProjectionCorrection::get_antenna_projection ()
{
  if (!built)
    build();

  return antenna_projection;
}

//! Get the transformation = feed_projection * antenna_projection
LabelledJones<double> VariableProjectionCorrection::get_value ()
{
  if (!built)
    build();

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
  // cerr << "VariableProjectionCorrection::build subint=" << subint << " built=" << built << endl;

  if (built)
    return;

  // use the ProjectionCorrection class to calculate the transformation
  correction.set_archive (archive);

  correction.required(subint);
  
  feed_projection = correction.get_feed_projection();
  feed_projection.label = correction.get_short_summary();

  correction.reset_summary();
  antenna_projection = correction.get_antenna_projection();
  antenna_projection.label = correction.get_short_summary();

  correction.reset_summary();
  transformation = correction (subint);
  description = correction.get_summary();
  transformation.label = correction.get_short_summary();
  
  built = true;
}
catch (Error& error)
{
  throw error += "VariableProjectionCorrection::build";
}
