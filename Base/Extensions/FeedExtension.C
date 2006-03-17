/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FeedExtension.h"

//! Default constructor
Pulsar::FeedExtension::FeedExtension ()
  : Extension ("Feed")
{
}

//! Copy constructor
Pulsar::FeedExtension::FeedExtension (const FeedExtension& extension)
  : Extension ("Feed")
{
  transformation = extension.transformation;
}

//! Operator =
const Pulsar::FeedExtension&
Pulsar::FeedExtension::operator= (const FeedExtension& extension)
{
  transformation = extension.transformation;
  return *this;
}

//! Destructor
Pulsar::FeedExtension::~FeedExtension ()
{
}
