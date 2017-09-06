//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Extensions/Pulsar/ObsExtension.h

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

    //! Return an abbreviated name that can be typed relatively quickly
    std::string get_short_name () const { return "obs"; }

    // get methods
    std::string get_observer() const { return observer; }
    void set_observer(const std::string& _observer) { observer = _observer; }

    std::string get_affiliate() const { return affiliation; }

    std::string get_project_ID() const { return project_ID; }
    void set_project_ID (const std::string& pid) { project_ID = pid; }

    //////////////////////////////////////////////////////////////

    //! Observer name
    std::string observer;
    
    //! Observer's affilitation
    std::string affiliation;

    //! Project ID
    std::string project_ID;
  };
 
}

#endif
