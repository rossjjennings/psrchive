//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Classes/Pulsar/Application.h

#ifndef __Pulsar_Application_h
#define __Pulsar_Application_h

#include "Pulsar/Processor.h"
#include "CommandLine.h"
#include "Functor.h"

#include <algorithm>

namespace Pulsar {

  class Archive;

  //! Application with basic command line options
  class Application : public Reference::Able
  {
  public:

    //! Construct with the application name and a short description
    Application (const std::string& name,
		 const std::string& description);

    //! Execute the main loop
    virtual int main (int argc, char** argv);

    //! Application options
    class Options;

    //! Add options to the application
    void add (Options*);

    //! Get the application name
    std::string get_name () const;

    //! Get the application description
    std::string get_description () const;

    //! Get the verbosity flag
    bool get_verbose () const;

    //! Return true if the application needs to save data
    virtual bool must_save ();

  protected:

    //! Available options
    std::vector< Reference::To<Options> > options;

    //! Add command line options
    virtual void add_options (CommandLine::Menu&) = 0;

    //! Parse the command line options
    virtual void parse (int argc, char** argv);

    //! Operate in quiet mode
    virtual void set_quiet ();

    //! Operate in verbose mode
    virtual void set_verbose ();

    //! Operate in very verbose mode
    virtual void set_very_verbose ();

    //! Any extra setup before running main loop
    virtual void setup ();

    template<typename Compare>
    void sort_archives (Compare c);
    
    //! The main loop
    virtual void run ();

    //! Load file
    virtual Archive* load (const std::string& filename);

    //! Data analysis tasks implemented by most derived classes
    virtual void process (Archive*) = 0;

    //! Return true if Options::finish tasks should be run after process
    virtual bool do_finish () { return true; }

    //! Return pointer to new result constructed by process method
    /*! 
      The result method was added to enable out-of-place process
      methods without changing the interface of the base class.
    */
    virtual Archive* result () { return 0; }

    //! Finishing work performed on each output archive
    virtual void finish (Archive*);

    //! Any final work after main loop finishes
    virtual void finalize ();

    //! true if application has an online manual
    bool has_manual;

    //! true if application should update the processing history
    bool update_history;

    //! true if application receives a script name as the first file
    bool stow_script;

    // name of the application
    std::string name;

    // short description of the application
    std::string description;

    // revision information
    std::string version;

    // command line used to execute this program
    std::string command;

    // filename of script
    std::string script;

    // list of file names on which to operate
    std::vector <std::string> filenames;

    // vector of loaded archives
    std::vector < Reference::To<Archive> > archives;
    
    // name of file containing list of Archive filenames
    std::string metafile;

    // verbosity flags
    bool verbose;
    bool very_verbose;

    // sort filenames returned by dirglob
    bool sort_filenames;
  };

  //! Describes application command line options.
  class Application::Options : public Processor
  {
    public:

    //! Additional getopt options
    virtual void add_options (CommandLine::Menu&) = 0;

    //! Additional one-time setup tasks
    virtual void setup ();

    //! Additional per-Archive processing tasks
    virtual void process (Archive*);

    //! Additional per-Archive finishing tasks (e.g., unload)
    virtual void finish (Archive*);

    //! Additional one-time final tasks
    virtual void finalize();

  protected:

    friend class Application;

    //! The application to which these options belong
    Reference::To<Application,false> application;

  };

  template<typename Compare>
  void Application::sort_archives (Compare compare)
  {
    archives.resize( filenames.size() );
    for (unsigned ifile=0; ifile < filenames.size(); ifile++)
      archives[ifile] = load (filenames[ifile]);

    std::sort (archives.begin(), archives.end(), compare);
  }

}

#endif
