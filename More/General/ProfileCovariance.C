/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ****************************************************************************/

#include "Pulsar/ProfileCovariance.h"

//! Default constructor
Pulsar::ProfileCovariance::ProfileCovariance ()
{
  rank = 0;
  count = 0;
  wt_sum = 0.0;
  wt_sum2 = 0.0;

  finalized = false;
}
