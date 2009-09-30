//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Timing/Pulsar/SincInterpShift.h,v $
   $Revision: 1.1 $
   $Date: 2009/09/30 02:17:24 $
   $Author: straten $ */

#ifndef __Pulsar_SincInterpShift_h
#define __Pulsar_SincInterpShift_h

#include "Pulsar/ProfileStandardShift.h"

namespace Pulsar {

  //! Estimates phase shift via sinc interpolation of CCF
  class SincInterpShift : public ProfileStandardShift
  {

  public:

    //! Fundamental period of spike sequence to be zapped
    static unsigned zap_period;

    //! Return the shift estimate
    Estimate<double> get_shift () const;

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface () { return 0; }

    //! Return a copy constructed instance of self
    SincInterpShift* clone () const { return new SincInterpShift(*this); }

  };

}


#endif // !defined __Pulsar_SincInterpShift_h
