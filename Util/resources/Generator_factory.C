/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Pulsar/Generator.h"

#ifdef HAVE_TEMPO2
#include "T2Generator.h"
#endif
#include "Predict.h"

using namespace std;

Pulsar::Generator* Pulsar::Generator::factory (const Pulsar::Parameters* param)
{
#ifdef HAVE_TEMPO2
  return new Tempo2::Generator;
#endif
  return new Tempo::Predict;
}
