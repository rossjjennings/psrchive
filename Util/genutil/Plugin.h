//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Plugin.h,v $
   $Revision: 1.2 $
   $Date: 2003/03/24 03:17:51 $
   $Author: pulsar $ */

#ifndef __Plugin_h
#define __Plugin_h

#include <vector>
#include <string>

#include "psr_cpp.h"

namespace Registry {

  class Plugin {

  public:
    //! Verbosity flag
    static bool verbose;

    //! Null constructor
    Plugin () { }

    //! Load all shared object libraries in the specified directory
    Plugin (const char* path) { load (path); }

    //! Load all shared object libraries in the specified directory
    void load (const char* path);

    //! Convenience interface
    void load (const string& path) { load (path.c_str()); }

    //! List of successfully loaded DSOs
    vector<string> ok;

    //! List of unsuccessfully loaded DSOs
    vector<string> fail;

  };

}

#endif
