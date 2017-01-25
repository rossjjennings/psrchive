//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2010 by Patrick Weltevrede
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/RemoveVariableBaseline.h

#ifndef _Pulsar_RemoveVariableBaseline_H
#define _Pulsar_RemoveVariableBaseline_H

#include "Pulsar/RemoveBaseline.h"

namespace Pulsar
{

  //! Remove a variable baseline from each  profile
  /*! This algorithm does not assume anything about the timing model
    or the dispersion measure.  The variable baseline is separately
    determined for each sub-integration, frequency channel, and
    polarization. */
  class RemoveVariableBaseline : public RemoveBaseline
  {
  public:

    //! Remove the baseline
    void transform (Archive*);
  };
}

#endif

