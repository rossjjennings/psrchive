/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ArchiveExtension.h"
#include "Pulsar/DigitiserCounts.h"

using namespace Pulsar;
using FrequencyAppend = Archive::Extension::FrequencyAppend;

static std::vector< const FrequencyAppend* >* instances = NULL;

static void build ()
{
  if (instances != NULL)
    return;

  // cerr << "FrequencyAppend::build" << endl;
 
  instances = new std::vector< const FrequencyAppend* >;
  
  instances->push_back( new DigitiserCounts::FrequencyAppend );
}


const std::vector< const FrequencyAppend* >& FrequencyAppend::children ()
{
  if (instances == NULL)
    build ();

  assert (instances != NULL);

  return *instances;
}

