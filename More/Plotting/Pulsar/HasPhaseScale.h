//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/Pulsar/HasPhaseScale.h

#ifndef __Pulsar_HasPhaseScale_h
#define __Pulsar_HasPhaseScale_h

namespace Pulsar {

  class PhaseScale;

  //! Interface to plots with a phase scale
  class HasPhaseScale
  {
  public:
    //! Get the scale
    virtual PhaseScale* get_scale () = 0;;
  };

}

#endif
