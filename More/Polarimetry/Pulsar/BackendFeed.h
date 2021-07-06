//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/BackendFeed.h

#ifndef __CalibrationBackendFeed_H
#define __CalibrationBackendFeed_H

#include "Pulsar/VariableBackend.h"

namespace Calibration {

  class Feed;

  //! Physical parameterization of the instrumental response

  /*! Abstract base class of instrumental response parameterizations
   that separate the backend and frontend transformations. */

  class BackendFeed : public VariableBackend
  {

  public:

    //! Fix the orientation of the frontend
    virtual void set_constant_orientation (bool flag = true) = 0;
    
    //! Provide access to the Feed model
    virtual const MEAL::Complex2* get_frontend () const = 0;
    virtual MEAL::Complex2* get_frontend ();

  };

}

#endif

