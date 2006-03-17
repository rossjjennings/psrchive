//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Plugin.h,v $
   $Revision: 1.3 $
   $Date: 2006/03/17 13:35:06 $
   $Author: straten $ */

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
