//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/ObsExtension.h,v $
   $Revision: 1.5 $
   $Date: 2006/10/06 21:05:50 $
   $Author: straten $ */

#ifndef __ObsExtension_h
#define __ObsExtension_h

#include "Pulsar/Archive.h"

namespace Pulsar {
  
  //! Observation Information Extension
  /*! This ObsExtension class provides a container for
    specific observation details like the name of the
    observer, the ID of the project and so on. */
  
  class ObsExtension : public Pulsar::Archive::Extension {
    
  public:
    
    //! Default constructor
    ObsExtension ();
    
    //! Copy constructor
    ObsExtension (const ObsExtension& extension);
    
    //! Operator =
    const ObsExtension& operator= (const ObsExtension& extension);
    
    //! Destructor
    ~ObsExtension ();

    //! Clone method
    ObsExtension* clone () const { return new ObsExtension( *this ); }

    //////////////////////////////////////////////////////////////

    //! Observer name
    std::string observer;
    
    //! Observer's affilitation
    std::string affiliation;

    //! Project ID
    std::string project_ID;
    
    //! Name of the telescope
    std::string telescope;

  };
 
}

#endif
