//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_Editor_h
#define __Pulsar_Editor_h

#include "Reference.h"
#include <string>
#include <vector>

namespace Pulsar {

  class Archive;

  //! Edit the metadata in a pulsar archive
  class Editor : public Reference::Able
  {

  public:

    Editor ();
    
    //! Add a semi-colon or comma-separated list of commands to execute
    void add_commands (const std::string&);

    //! Add a semi-colon or comma-separated list of extensions to install
    void add_extensions (const std::string&);

    //! Return true if the process method will modify the archive
    bool will_modify () const;

    //! Edit the Archive according to the current state
    std::string process (Archive*);

    //! prefix parameter value queries with parameter name=
    bool prefix_name;

    // print the name of each file processed
    bool output_filename;

  protected:

    //! commands to be executed
    std::vector<std::string> commands;

    //! extensions to be added
    std::vector<std::string> extensions;

  };

}

#endif
