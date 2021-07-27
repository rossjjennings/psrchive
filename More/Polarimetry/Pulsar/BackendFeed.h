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

#include "MEAL/ProductRule.h"
#include "MEAL/Complex2.h"

namespace Calibration {

  class VariableBackend;
  
  //! Physical parameterization of the instrumental response

  /*! Abstract base class of instrumental response parameterizations
   that separate the backend and frontend transformations. */

  class BackendFeed : public MEAL::ProductRule<MEAL::Complex2>
  {
  private:
    
    void init ();

    Reference::To<VariableBackend> backend;
    Reference::To<MEAL::Complex2> frontend;
    
  protected:

    void set_frontend (MEAL::Complex2*);
    
  public:

    //! Default constructor
    BackendFeed ();

    //! Copy Constructor
    BackendFeed (const BackendFeed& s);

    //! Assignment Operator
    const BackendFeed& operator = (const BackendFeed& s);

    //! Destructor
    ~BackendFeed ();

    //! Fix the orientation of the frontend
    virtual void set_constant_orientation (bool flag = true) = 0;

    //! Set cyclical bounds on angular quantities
    virtual void set_cyclic (bool flag);

    //! Provide access to the backend model
    const VariableBackend* get_backend () const;
    VariableBackend* get_backend ();

    //! Provide access to the frontend model
    const MEAL::Complex2* get_frontend () const { return frontend; }
    MEAL::Complex2* get_frontend () { return frontend; }
  };

}

#endif

