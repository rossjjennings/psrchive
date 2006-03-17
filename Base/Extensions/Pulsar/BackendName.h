//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/BackendName.h,v $
   $Revision: 1.2 $
   $Date: 2006/03/17 13:34:45 $
   $Author: straten $ */

#ifndef __Pulsar_BackendName_h
#define __Pulsar_BackendName_h

#include "Pulsar/Backend.h"

namespace Pulsar {
  
  //! A named Backend Extension
  /*! This class implements only the Backend name */
  class BackendName : public Pulsar::Backend {

  public:
    
    //! Default constructor
    BackendName ();

    //! Copy constructor
    BackendName (const BackendName& extension);

    //! Assignment operator
    const BackendName& operator= (const BackendName& extension);

    //! Destructor
    ~BackendName ();

    //////////////////////////////////////////////////////////////////////
    //
    // Archive::Extension implementation
    //
    //////////////////////////////////////////////////////////////////////

    //! Clone method
    BackendName* clone () const
    { return new BackendName( *this ); }

    //////////////////////////////////////////////////////////////////////
    //
    // Backend implementation
    //
    //////////////////////////////////////////////////////////////////////
    
    //! Get the name of the Backend
    string get_name () const { return name; }

    //! Set the name of the backend
    void set_name (const string& _name) { name = _name; }

  protected:

    //! The name of the backend
    string name;


  };
 

}

#endif
