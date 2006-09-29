/***************************************************************************
 *
 *   Copyright (C) 2000 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifndef __DIRUTILS_H
#define __DIRUTILS_H

#ifdef __cplusplus

#include <vector>
#include <string>

// in dirwith.C
const std::string dirwith (double size, const std::vector<std::string>& disks,
		      const std::string& path=std::string(),
		      double usage_limit = 0.0,
		      double leave_free = 0.0);

// in dirlist.C
int dirlist (std::vector<std::string>* files, const char* path, bool all=false,
	     const char* pattern=NULL);
int dirlist (std::vector<std::string>* files, const std::string& path, bool all=false,
	     const char* pattern=NULL);
int dirlist (std::vector<std::string>* files, const std::vector<std::string>& paths,
	     bool all=false, const char* pattern=NULL);

// in dirtree.C
int dirtree (std::vector<std::string>* files, const char* path,
	     const char* pattern=NULL);
int dirtree (std::vector<std::string>* files, const std::vector<std::string>& paths,
	     const char* pattern=NULL);

// in dirglob.C
bool is_glob_argument (const char* text);
bool is_glob_argument (const std::string& text);

void dirglob (std::vector<std::string>* filenames, const char* text);
void dirglob (std::vector<std::string>* filenames, const std::string& text);

// in dirglobtree.C
// recursively follows directories
void dirglobtree (std::vector<std::string>* filenames, 
		  const std::string& root, const std::vector<std::string>& patterns);

void dirglobtree (std::vector<std::string>* filenames, 
		  const std::string& root, const std::string& pattern);

extern "C" {
#endif /* #ifdef __cplusplus */

  /* recursively constructs a new directory named 'path' */
  int makedir   (const char* path);
  /* resursively removes an existing directory named 'path' */
  int removedir (const char* path);

#ifdef __cplusplus
}
#endif

#endif
