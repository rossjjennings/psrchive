//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Timing/Pulsar/ShiftEstimator.h,v $
   $Revision: 1.1 $
   $Date: 2009/09/29 22:22:09 $
   $Author: straten $ */

#ifndef __Pulsar_ShiftEstimator_h
#define __Pulsar_ShiftEstimator_h

#include "Pulsar/Algorithm.h"

#include "TextInterface.h"
#include "Estimate.h"

namespace Pulsar {

  //! Algorithms that estimate pulse phase shifts
  class ShiftEstimator : public Algorithm
  {

  public:

    //! Return the shift estimate
    virtual Estimate<double> get_shift () const = 0;

    //! Return a text interface that can be used to configure this instance
    virtual TextInterface::Parser* get_interface () = 0;

    //! Return a copy constructed instance of self
    virtual ShiftEstimator* clone () const = 0;

    //! Construct a new ShiftEstimator from a string
    static ShiftEstimator* factory (const std::string& name_and_parse);

  };

}


#endif // !defined __Pulsar_ShiftEstimator_h
