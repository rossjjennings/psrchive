//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Timing/Pulsar/ZeroPadShift.h

#ifndef __Pulsar_ZeroPadShift_h
#define __Pulsar_ZeroPadShift_h

#include "Pulsar/ProfileStandardShift.h"
#include "Pulsar/Config.h"

namespace Pulsar {

  //! Estimates phase shift in Fourier domain using zero-pad interpolation
  class ZeroPadShift : public ProfileStandardShift
  {

  public:

    //! Interpolate by this factor
    static Option<unsigned> interpolate;

    //! Return the shift estimate
    Estimate<double> get_shift () const;

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ()
    { return new EmptyInterface<ZeroPadShift> ("ZPS"); }

    //! Return a copy constructed instance of self
    ZeroPadShift* clone () const { return new ZeroPadShift(*this); }

  };

}


#endif // !defined __Pulsar_ZeroPadShift_h
