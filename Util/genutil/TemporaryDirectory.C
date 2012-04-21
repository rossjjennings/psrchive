/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "TemporaryDirectory.h"
#include "dirutil.h"
#include "Error.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>
using namespace std;

TemporaryDirectory::TemporaryDirectory (const std::string& basename)
{
  string root = "/tmp/" + basename;

  const char* unknown = "unknown";
  const char* userid = getenv ("USER");
  if (!userid)
    userid = unknown;

  path = (root + "/") + userid;

  if (makedir (path.c_str()) < 0)
  {
    cerr << "TemporaryDirectory: failure creating '" << path << "'" << endl;

    char* home = getenv ("HOME");

    if (home)
      path = home;
    else
      path = ".";

    path += "/" + basename + ".tmp";

    if (makedir (path.c_str()) < 0)
      throw Error (InvalidState, "TemporaryDirectory",
		   "cannot create a temporary working path");
  }
  else
  {
    /* the first mkdir was successful; ensure that others can create
       temporary directories at the same root */

    mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
    chmod (root.c_str(), mode);
  }
}
