/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Attic/string_utils.h,v $
   $Revision: 1.9 $
   $Date: 2000/01/24 12:21:04 $
   $Author: redwards $ */

#ifndef __STRING_UTILS_H
#define __STRING_UTILS_H

#include <sys/types.h>
#include <stdio.h>

#include <iostream>
#include <string>
#include <vector>

#include "psr_cpp.h"

// ///////////////////////////////////////////////////////////
// reads up to 'nbytes' (or to eof) from 'istr', adding them 
// to 'str'.  returns number of bytes read or -1 on error
// ///////////////////////////////////////////////////////////
ssize_t stringload (string* str, istream &istr, streamsize nbytes=0);

// ///////////////////////////////////////////////////////////
// reads up to 'nbytes' (or to eof) from 'fptr', adding them 
// to 'str'.  returns number bytes read or -1 on error
// ///////////////////////////////////////////////////////////
ssize_t stringload (string* str, FILE* fptr, size_t nbytes=0);

// ///////////////////////////////////////////////////////////
// fills a vector of string with the first word from each line
// in the file.  A line is delimited by \n or commented by #.
// ///////////////////////////////////////////////////////////
int stringfload (vector<string>* lines, const char* filename);
int stringload (vector<string>* lines, FILE* fptr);

// ///////////////////////////////////////////////////////////
// returns the first sub-string of 'instr' delimited by
// characters in 'delimiters'.  the substring and any leading
// delimiter characters are removed from 'instr'
// ///////////////////////////////////////////////////////////
string stringtok (string * instr, const string & delimiters);

// ///////////////////////////////////////////////////////////
// other interfaces to overload stringtok()
inline string stringtok (string* instr, char* delimiters)
{ return stringtok (instr, string(delimiters)); }

inline string stringtok (string* instr, char delimiter)
{ return stringtok (instr, string(1, delimiter)); }

// ///////////////////////////////////////////////////////////
// returns the first sub-string of 'instr' delimited by
// 'delimiter'.
// ///////////////////////////////////////////////////////////
string subdlim (const string& instr, const string& delimiter);

inline string subdlim (const string& instr, char* delimiters)
{ return subdlim (instr, string(delimiters)); }

inline string subdlim (const string& instr, char delimiter)
{ return subdlim (instr, string(1, delimiter)); }

// ///////////////////////////////////////////////////////////
int stringlen (double val, unsigned precision = 15);

inline int stringlen (int val, unsigned precision = 0)
{ return stringlen (double(val), precision); }

inline int stringlen (float val, unsigned precision = 6)
{ return stringlen (double(val), precision); }

string stringprintf(char *fmt ...);

vector<string> 
   stringdecimate(const string& wordstr, const string& delimiters);
string stringdelimit(const vector<string>& words, char delimiter);




#endif

