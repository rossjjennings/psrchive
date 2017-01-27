//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/morphological_difference.h

#ifndef __morphological_difference_h
#define __morphological_difference_h

namespace Pulsar {

  class Profile;

  Profile* morphological_difference (const Profile* p1, const Profile* p2,
                                     double& scale, double& shift,
                                     float phs1 = 0, float phs2 = 1.0);
}

#endif
