//-*-C++-*-
/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Attic/string_utils.h,v $
   $Revision: 1.13 $
   $Date: 2002/08/14 18:42:06 $
   $Author: hknight $ */

#ifndef __STRING_UTILS_H
#define __STRING_UTILS_H

#include <sys/types.h>
#include <stdio.h>

#include <iostream>
#include <strstream>
#include <string>
#include <vector>

#include "psr_cpp.h"

// Like perl chomp.  Returns true if successful.
bool h_chomp(string& ss, char gone='\n');
bool h_chomp(string& ss, string gone);

// Like perl chop
void h_chop(string& ss);

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
string stringtok (string * instr, const string & delimiters,
		  bool skip_leading_delimiters = true,
		  bool strip_leading_delimiters_from_remainder = true);

// ///////////////////////////////////////////////////////////
// other interfaces to overload stringtok()
inline string stringtok (string* instr, char* delimiters,
			 bool skip_leading_delimiters = true,
			 bool strip_leading_delimiters_from_remainder = true)
{ return stringtok (instr, string(delimiters),
		    skip_leading_delimiters, 
		    strip_leading_delimiters_from_remainder); }

inline string stringtok (string* instr, char delimiter,
			 bool skip_leading_delimiters = true,
			 bool strip_leading_delimiters_from_remainder = true)
{ return stringtok (instr, string(1, delimiter),
		    skip_leading_delimiters,
		    strip_leading_delimiters_from_remainder); }

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

// Stuff to delimit a vector of strings and the reverse
vector<string> 
   stringdecimate(const string& wordstr, const string& delimiters);
string stringdelimit(const vector<string>& words, char delimiter);

// Stuff to turn an array of char *'s into a vector of strings
// useful for taking lists of files on the command line
vector<string> cstrarray2vec(const char **vals, int nelem);

template<class T>
string make_string(T input){
  ostrstream ost;
  ost << input << ends;
  return ost.str();
}

template<class T>
T convert_string(string ss){
  ostrstream ost;
  ost << ss << ends;
  istrstream ist(ost.str());
  
  T outie;
  ist >> outie; 
  return outie;
}

#endif


