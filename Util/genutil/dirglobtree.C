#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>

#include "dirutil.h"
#include "genutil.h"

void dirglobtree (vector<string>* filenames, 
		  const string& root, const string& pattern)
{
  string path = root;
  if (path.length())
    path += "/";

  dirglob (filenames, path + pattern);

  string current = root;
  if (!root.length())
    current = ".";

  DIR* dird = opendir (current.c_str());
  if (dird == NULL) {
    fprintf (stderr, "dirglobtree: Error opening directory:'%s'",
	     current.c_str());
    perror ("");
    return;
  }

  struct dirent *entry;
 
  while ((entry = readdir (dird)) != NULL) {
    string name (entry->d_name);
    string next = path + name;
    if (name != "." && name != ".." && file_is_directory (next.c_str()))
      dirglobtree (filenames, next, pattern);
  }

  closedir (dird);
}
