/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/BackendFeed.h"

using namespace std;

MEAL::Complex2* Calibration::BackendFeed::get_frontend ()
{
  const BackendFeed* thiz = this;
  return const_cast<MEAL::Complex2*>( thiz->get_frontend() );
}
