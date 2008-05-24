/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tempo++.h"

#ifdef HAVE_TEMPO2
//#include "T2Observatory.h"
#endif

using namespace std;

const Pulsar::Site* 
Pulsar::Site::location (const string& antenna)
{
#ifdef HAVE_TEMPO2
  //try {
  //  return Tempo2::observatory (antenna);
  //}
  //catch (Error)
  //{
  //}
#endif

  return Tempo::observatory (antenna);
}
