//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Backend.h,v $
   $Revision: 1.5 $
   $Date: 2006/09/07 15:50:28 $
   $Author: straten $ */

#ifndef __Pulsar_Backend_h
#define __Pulsar_Backend_h

#include "Pulsar/Archive.h"
#include "Conventions.h"

namespace Pulsar {
  
  //! Stores information about the instrument backend
  /*! Derived classes must implement clone and get_name methods. */
  class Backend : public Archive::Extension {

  public:
    
    //! Default constructor
    Backend (const char* name);

    //! Copy constructor
    Backend (const Backend&);

    //! Operator =
    const Backend& operator= (const Backend&);

    //! Return the name of the Backend
    virtual string get_name () const = 0;

    //! Get the hand of the backend
    Signal::Hand get_hand () const;

    //! Set the hand of the backend
    void set_hand (Signal::Hand);

    //! Get the argument of the backend
    Signal::Argument get_argument () const;

    //! Set the argument of the backend
    void set_argument (Signal::Argument);

    //! True if backend has compensated for lower sideband downconversion
    bool get_downconversion_corrected () const;

    //! True if backend has compensated for lower sideband downconversion
    void set_downconversion_corrected (bool corrected = true);

  protected:

    //! The bandend hand
    Signal::Hand hand;

    //! The backend argument
    Signal::Argument argument;

    //! True if backend has compensated for lower sideband downconversion
    bool downconversion_corrected;

  };
 

}

#endif
