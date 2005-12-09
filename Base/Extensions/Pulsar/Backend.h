//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Backend.h,v $
   $Revision: 1.2 $
   $Date: 2005/12/09 16:41:06 $
   $Author: straten $ */

#ifndef __Pulsar_Backend_h
#define __Pulsar_Backend_h

#include "Pulsar/Archive.h"

namespace Pulsar {
  
  //! Stores information about the instrument backend
  /*! This Archive::Extension is an abstract base for classes that
    contain information specific to the backend instrument used for
    the observation. */
  class Backend : public Archive::Extension {

  public:
    
    //! Default constructor
    Backend (const char* name) : Extension (name) { }

    //! Return the name of the Backend
    virtual string get_name () const = 0;

  };
 

}

#endif
