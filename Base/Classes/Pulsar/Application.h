//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Application.h,v $
   $Revision: 1.3 $
   $Date: 2008/01/17 21:36:49 $
   $Author: straten $ */

#ifndef __Pulsar_Application_h
#define __Pulsar_Application_h

#include "Reference.h"
#include <string>

namespace Pulsar {

  class Archive;

  //! Applications with the basic command line options
  class Application : public Reference::Able
  {
  public:

    //! Construct with the application name and a short description
    Application (const std::string& name, const std::string& description);

    //! Execute the main loop
    virtual int main (int argc, char** argv);

    //! Application features
    class Feature;

    //! Add a feature to the application
    void add (Feature*);

    //! Get the application name
    std::string get_name () const;

    //! Get the application description
    std::string get_description () const;

    //! Get the verbosity flag
    bool get_verbose () const;

  protected:

    //! Available features
    std::vector< Reference::To<Feature> > features;

    //! Provide usage information
    virtual void usage ();

    //! Additional usage information implemented by derived classes
    virtual std::string get_usage ();

    //! Additional getopt options
    virtual std::string get_options ();

    //! Parse the command line options
    virtual void parse (int argc, char** argv);

    //! Parse an additional command, return true if understood
    virtual bool parse (char code, const std::string& arg);

    //! Data analysis tasks implemented by most derived classes
    virtual void process (Archive*) = 0;

    //! Set to true if this application has an online manual
    bool has_manual;

    // name of the application
    std::string name;

    // short description of the application
    std::string description;

    // list of file names on which to operate
    std::vector <std::string> filenames;

    // name of file containing list of Archive filenames
    char* metafile;

    // verbosity flags
    bool verbose;
    bool very_verbose;

  };

  class Application::Feature : public Reference::Able
  {
    public:

    //! Additional usage information
    virtual std::string get_usage () = 0;

    //! Additional getopt options
    virtual std::string get_options () = 0;

    //! Parse an additional command, return true if understood
    virtual bool parse (char code, const std::string& arg) = 0;

    //! Additional setup tasks
    virtual void setup ();

    //! Additional processing tasks
    virtual void process (Archive*);

  protected:

    friend class Application;

    //! The application to which this feature belongs
    Reference::To<Application,false> application;

  };

}

#endif
