//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/ExampleExtension.h,v $
   $Revision: 1.1 $
   $Date: 2003/06/12 18:40:00 $
   $Author: straten $ */

#ifndef __ExampleExtension_h
#define __ExampleExtension_h

#include "Pulsar/Archive.h"

namespace Pulsar {

  //! Example Pulsar::Archive Extension
  /*! This ExampleExtension class provides an example of how to
    inherit the Pulsar::Archive::Extension class. By copying the files
    ExampleExtension.h and ExampleExtension.C and performing simple
    text-substitution, the skeleton of a new Extension may be easily
    developed. */
  class ExampleExtension : public Pulsar::Archive::Extension {

  public:
    
    //! Default constructor
    ExampleExtension ();

    //! Copy constructor
    ExampleExtension (const ExampleExtension& extension);

    //! Operator =
    const ExampleExtension& operator= (const ExampleExtension& extension);

    //! Destructor
    ~ExampleExtension ();

  };
 

}

#endif
