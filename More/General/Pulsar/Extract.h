//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2014 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/Extract.h

#ifndef __Pulsar_Extract_h
#define __Pulsar_Extract_h

#include "Pulsar/Transformation.h"
#include "Pulsar/Profile.h"
#include <utility>

namespace Pulsar {

  //! Extracts (and keeps) a specified range of phase bins
  class Extract : public Transformation<Profile>
  {
  public:

    typedef std::pair<unsigned,unsigned> Range;

    Extract (const Range&);
    void transform (Profile*);

  protected:
    Range range;

  }; 

}

#endif
