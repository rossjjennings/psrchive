//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/ObsExtension.h,v $
   $Revision: 1.11 $
   $Date: 2008/04/21 06:20:00 $
   $Author: straten $ */

#ifndef __ObsExtension_h
#define __ObsExtension_h

#include "Pulsar/ArchiveExtension.h"
#include "TextInterface.h"

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
    
    //! Return a text interfaces that can be used to access this instance
    TextInterface::Parser* get_interface();

    // Text interface to a ObsExtension object
    class Interface : public TextInterface::To<ObsExtension>
    {
      public:
	Interface( ObsExtension *s_instance = NULL );
    };

    // get methods
    std::string get_observer() const { return observer; }
    std::string get_affiliate() const { return affiliation; }
    std::string get_project_ID() const { return project_ID; }
    std::string get_telescope() const { return telescope; }

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
