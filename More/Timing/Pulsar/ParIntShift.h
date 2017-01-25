//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Timing/Pulsar/ParIntShift.h

#ifndef __Pulsar_ParIntShift_h
#define __Pulsar_ParIntShift_h

#include "Pulsar/ProfileStandardShift.h"

namespace Pulsar {

  //! Estimates phase shift in time domain using parabolic interpolation
  class ParIntShift : public ProfileStandardShift
  {

  public:

    //! Return the shift estimate
    Estimate<double> get_shift () const;

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ()
    { return new EmptyInterface<ParIntShift> ("PIS"); }

    //! Return a copy constructed instance of self
    ParIntShift* clone () const { return new ParIntShift(*this); }

  };

}


#endif // !defined __Pulsar_ParIntShift_h
