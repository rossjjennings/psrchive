#include <iostream>
#include <vector>
#include <string>

#include <glob.h>
#include <string.h>

#include "dirutil.h"

bool is_glob_argument (const char* text)
{
  if (strchr (text, '?'))
    return true;
  if (strchr (text, '*'))
    return true;
  if (strchr (text, '['))
    return true;

  return false;
}

bool is_glob_argument (const string& text)
{
  return is_glob_argument (text.c_str());
}
 
void dirglob (vector<string>* filenames, const char* text)
{
  glob_t rglob;

  int ret = glob (text, GLOB_NOSORT, NULL, &rglob);
  if (ret != 0 && ret != GLOB_NOMATCH)  {
    string error ("dirglob: error calling glob");
    cerr << error << endl;
    throw (error);
  }
  for (size_t ifile=0; ifile < rglob.gl_pathc; ifile++)
    filenames->push_back (string(rglob.gl_pathv[ifile]));

  globfree (&rglob);
}

void dirglob (vector<string>* filenames, const string& text)
{
  dirglob (filenames, text.c_str());
}

