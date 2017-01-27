//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/find_spike_edges.h

#ifndef __find_spike_edges_h
#define __find_spike_edges_h

namespace Pulsar {

  class Profile;

  //! Find the bin numbers at which the flux falls below a threshold
  void find_spike_edges(const Profile* profile, int& rise, int& fall,
                        float pc = 0.2, int spike_bin = -1);

}

#endif

