/***************************************************************************
 *
 *   Copyright (C) 2004 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

/*! Rotate pulsar Integrations so that the bin of largest amplitude
    is centred */
void Pulsar::Archive::centre_max_bin ()
{
  Reference::To<Pulsar::Archive> arch = total();
  double p = arch->get_Integration(0)->get_folding_period();

  int bnum = arch->get_Profile(0,0,0)->find_max_bin();
  bnum -= get_nbin()/2;

  float frac = float(bnum)/float(get_nbin());
  double extra_time = frac * p;

  rotate(extra_time); 
}
