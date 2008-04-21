//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Backend.h,v $
   $Revision: 1.14 $
   $Date: 2008/04/21 01:58:35 $
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
    Backend (const std::string& ext_name = "Backend");

    //! Copy constructor
    Backend (const Backend&);

    //! Operator =
    const Backend& operator= (const Backend&);

    //! Clone method
    Backend* clone () const
    { return new Backend( *this ); }

    //! Return an abbreviated name that can be typed relatively quickly
    std::string get_short_name () const { return "be"; }

    // Text interface to a Backend extension
    class Interface : public TextInterface::To<Backend>
    {
      public:
	Interface( Backend *s_instance = NULL );
    };

    //! Return a text interfaces that can be used to access this instance
    TextInterface::Parser* get_interface();

    //////////////////////////////////////////////////////////////////////
    //
    // Backend implementation
    //
    //////////////////////////////////////////////////////////////////////

    //! Return the name of the Backend
    std::string get_name () const;

    //! Set the name of the backend
    void set_name (const std::string& name);

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
    
    //! Get the delay
    double get_delay() const { return delay; }
    
    //! Set the delay
    void set_delay( const double set_delay ) { delay = set_delay; }

  protected:

    //! The name of the backend
    std::string name;

    //! The bandend hand
    Signal::Hand hand;

    //! The backend argument
    Signal::Argument argument;

    //! True if backend has compensated for lower sideband downconversion
    bool downconversion_corrected;
    
    //! proportional delay from digitiser input
    double delay;

  };
 

}

#endif
