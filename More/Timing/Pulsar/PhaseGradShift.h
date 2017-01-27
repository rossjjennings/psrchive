//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Timing/Pulsar/PhaseGradShift.h

#ifndef __Pulsar_PhaseGradShift_h
#define __Pulsar_PhaseGradShift_h

#include "Pulsar/ProfileStandardShift.h"

namespace Pulsar {

  //! Estimates phase shift in Fourier domain
  class PhaseGradShift : public ProfileStandardShift
  {

  public:

    //! Return the shift estimate
    Estimate<double> get_shift () const;

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ()
    { return new EmptyInterface<PhaseGradShift> ("PGS"); }

    //! Return a copy constructed instance of self
    PhaseGradShift* clone () const { return new PhaseGradShift(*this); }

  };

}


#endif // !defined __Pulsar_PhaseGradShift_h
