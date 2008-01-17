//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Application.h,v $
   $Revision: 1.1 $
   $Date: 2008/01/17 11:31:10 $
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

    //! Construct with the application name
    Application (const std::string& name, const std::string& description);

    //! Execute the main loop
    virtual int main (int argc, char** argv);

  protected:

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

    //! Preprocessing tasks implemented by partially derived classes
    virtual void preprocess (Archive*);

    //! Data analysis tasks implemented by most derived classes
    virtual void process (Archive*) = 0;

    //! Set to true if this application has an online manual
    bool has_manual;

    // name of the application
    std::string application_name;

    // short description of the application
    std::string application_description;

    // list of file names on which to operate
    std::vector <std::string> filenames;

    // name of file containing list of Archive filenames
    char* metafile;

    // verbosity flag
    bool verbose;

  };

}

#endif
