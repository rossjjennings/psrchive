//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/StandardOptions.h

#ifndef __Pulsar_StandardOptions_h
#define __Pulsar_StandardOptions_h

#include "Pulsar/Application.h"

namespace Pulsar {

  class Interpreter;

  //! Standard interpreter command line options
  class StandardOptions : public Application::Options
  {
  public:

    //! Default constructor
    StandardOptions ();

    //! Preprocess the archive using the standard_shell interpreter
    void process (Archive*);

    //! Return the top of the interpreter stack
    Archive* result ();

    //! Add to the jobs
    virtual void add_job (const std::string& job);

    //! Load a script into the jobs
    virtual void add_script (const std::string& job);

    //! Add to the default preprocessing jobs (ignored if jobs are set)
    virtual void add_default_job (const std::string& job);

    //! Provide access to the interpreter
    virtual Interpreter* get_interpreter ();

  private:

    //! Add options to the menu
    void add_options (CommandLine::Menu&);

    // Preprocessing jobs
    std::vector<std::string> jobs;

    // Default preprocessing jobs
    std::vector<std::string> default_jobs;

    // standard interpreter
    Reference::To<Interpreter> interpreter;

    // Top of the interpreter stack after executing script
    Reference::To<Archive> the_result;

  };

}

#endif
