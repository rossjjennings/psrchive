//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Classes/Pulsar/Agent.h

#ifndef __Pulsar_Agent_h
#define __Pulsar_Agent_h

#include "Pulsar/Archive.h"
#include "Registry.h"

#include <vector>


namespace Pulsar {

  /* This abstract base class should be inherited through the
     Advocate template. */
  class Archive::Agent : public Reference::Able {

  public:
    
    // Destructor
    virtual ~Agent ();
    
    // Advocate the use of the derived class to interpret filename
    virtual bool advocate (const char* filename) = 0;
    
    // Return the name of the derived class
    virtual std::string get_name () = 0;
    
    // Return a description of the derived class
    virtual std::string get_description () = 0;
    
    // Return a null-constructed instance of the derived class
    virtual Archive* new_Archive () = 0;
    
    // Return the name of the plugins directory
    static std::string get_plugin_path (const char* shell_variable);
    
    // Report to cerr on the status of the Registry (and plugins)
    static void report ();

    // Return a string containing the list of plugins formatted for output
    static std::string get_list ();

    // Get a list of name,description pairs from the registry
    static void get_list( std::vector<std::pair<std::string,std::string> >& );

  protected:

    // The path from which plugin code will be loaded
    static std::string plugin_path;

    // Flag that plugin_load has been called
    static bool loaded;

    // Initialization function calls plugin_load if dynamic linkage enabled
    static bool init ();

  };

  /* This template base class must be inherited in order to register
     plugins for use in the Archive::load factory.  */
  template<class Plugin>
    class Archive::Advocate : public Agent {

  public:

    // Constructor ensures that template entry is instantiated
    Advocate () { entry.get(); }

    // Return a new instance of the Archive derived class
    Archive* new_Archive () { return new Plugin; }

    // ensure that the Advocate is linked into static binaries
    static void ensure_linkage () { entry.get(); }

  private:

    // Enter template constructor adds Advocate<Plugin> to Agent::registry
    static Registry::List<Archive::Agent>::Enter<typename Plugin::Agent> entry;

  };

#ifdef PSRCHIVE_PLUGIN
  template<class Plugin>
    Registry::List<Archive::Agent>::Enter<typename Plugin::Agent> 
    Archive::Advocate<Plugin>::entry;
#endif

}

#endif
