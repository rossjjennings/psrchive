//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Backend.h,v $
   $Revision: 1.1 $
   $Date: 2004/07/12 07:33:49 $
   $Author: straten $ */

#ifndef __Pulsar_Backend_h
#define __Pulsar_Backend_h

#include "Pulsar/Archive.h"

namespace Pulsar {
  
  //! Pure virtual base class of backend information Extensions
  /*! The Backend Extension is an abstract base for classes that
    contain information specific to the backend instrument used for
    the observation. */

  class Backend : public Pulsar::Archive::Extension {

  public:
    
    //! Default constructor
    Backend (const char* name) : Extension (name) { }

    //! Return the name of the Backend
    virtual string get_name () const = 0;

  };
 

}

#endif
