#ifndef __DIRUTILS_H
#define __DIRUTILS_H

#include <vector>
#include <string>

// in dirwith.C
const string dirwith (double size, const vector<string>& disks,
		      const string& path=string(),
		      double usage_limit = 0.0,
		      double leave_free = 0.0);

// in dirlist.C
int dirlist (vector<string>* files, const char* path, bool all=false,
	     const char* pattern=NULL);
int dirlist (vector<string>* files, const string& path, bool all=false,
	     const char* pattern=NULL);
int dirlist (vector<string>* files, const vector<string>& paths,
	     bool all=false, const char* pattern=NULL);

// in dirtree.C
int dirtree (vector<string>* files, const char* path,
	     const char* pattern=NULL);
int dirtree (vector<string>* files, const vector<string>& paths,
	     const char* pattern=NULL);

// in dirglob.C
bool is_glob_argument (const char* text);
bool is_glob_argument (const string& text);

void dirglob (vector<string>* filenames, const char* text);
void dirglob (vector<string>* filenames, const string& text);

#endif
