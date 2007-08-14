/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Predict.h"

#ifdef HAVE_TEMPO2
#include "T2Generator.h"
#endif

using namespace std;

Pulsar::Generator* Pulsar::Generator::get_default ()
{
#ifdef HAVE_TEMPO2
  return new Tempo2::Generator;
#endif
  return new Tempo::Predict;
}
