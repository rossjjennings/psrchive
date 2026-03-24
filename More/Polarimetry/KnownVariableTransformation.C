/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/KnownVariableTransformation.h"

using namespace Pulsar;

void KnownVariableTransformation::set_nchan (unsigned nchan)
{
  xform.resize(nchan);
}

KnownVariableTransformation::Transformation*
KnownVariableTransformation::get_transformation (unsigned ichan)
{
  if (ichan >= xform.size())
    throw Error (InvalidState, "Pulsar::KnownVariableTransformation::get_transformation",
      "chan=%u >= nchan=%u", ichan, xform.size());
  return &(xform[ichan]);
}

MEAL::Argument::Value* KnownVariableTransformation::new_value ()
{
  if (chan >= xform.size())
    throw Error (InvalidState, "Pulsar::KnownVariableTransformation::new_value",
      "chan=%u >= nchan=%u", chan, xform.size());

  return xform[chan].argument.new_Value( get_value() );
}
