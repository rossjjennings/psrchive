//-*-C++-*-
/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Attic/string_utils.h,v $
   $Revision: 1.24 $
   $Date: 2003/10/03 06:30:25 $
   $Author: hknight $ */

#ifndef __STRING_UTILS_H
#define __STRING_UTILS_H

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <iomanip>
#include <strstream>
//#include <sstream>
//#include <fstream>
//#include <ostream>
#include <string>
#include <vector>

#include "psr_cpp.h"

// Like perl chomp.  Returns true if successful.
bool h_chomp(string& ss, char gone='\n');
bool h_chomp(string& ss, string gone);

// Like perl chop
string& h_chop(string& ss);
// Exactly the same as h_chop()
string& chop(string& ss);

// Returns ss instead of a boolean value
string& chomp(string& ss, char gone='\n');
string& chomp(string& ss, string gone);

// Like fscanf(fptr,"%s%s",ignore,answer) except it gets more than 1 word
bool retrieve_cstring(FILE* fptr,string ignore,char* answer);

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
   stringdecimate(const string& wordstr, string delimiters);
string stringdelimit(const vector<string>& words, char delimiter);

// Returns one line per element of return vector
vector<string> stringlines(const string& str);

// Stuff to turn an array of char *'s into a vector of strings
// useful for taking lists of files on the command line
vector<string> cstrarray2vec(const char **vals, int nelem);

template<class T>
string form_string(T input){
  ostrstream ost;
  ost << input << ends;
  string ss = ost.str();
  delete ost.str();
  return ss;
}

template<class T>
string form_string(T input, int precision){
  ostrstream ost;
  ost << setprecision( precision ) << input << ends;
  string ss = ost.str();
  delete ost.str();
  return ss;
}

/* Doesn't work for pointers!  (Stupid linux compiler) */
template<class T>
T convert_string(string ss){
  ostrstream ost;
  ost << ss << ends;

  istrstream ist(ost.str());
  
  T outie;
  ist >> outie;

  delete ost.str();

  return outie;
}

/* Doesn't work for pointers!  (Stupid linux compiler) */
template<class T>
T convert_string(string ss, int precision){
  
  ostrstream ost;
  ost << setiosflags(ios::fixed) << setprecision( precision ) << ss << ends;

  istrstream ist(ost.str());
  ist >> setiosflags(ios::fixed) >> setiosflags(ios::showpoint) >> setprecision( precision );

  T outie;
  ist >> setw( precision ) >> outie; 
  
  delete ost.str();
  return outie;
}

string bool2string(bool mybool);
void* string2ptr(string ss);

/* these use stdio- which doesn't use costly dynamic memory allocations */
string make_string(int input);
string make_string(int input, int places);
string make_string(unsigned input);
string make_string(unsigned input, int places);
string make_string(long input);
string make_string(unsigned long input);
string make_string(long long input);
string make_string(unsigned long long input);
string make_string(float input);
string make_string(float input, int decimal_places);
string make_string(double input);
string make_string(double input, int decimal_places);
string make_string(void* input);
// just incase someone is dumb enough...
string make_string(char input);
// just incase someone is CRAZY enough - writes as an int!
string make_string(unsigned char input);
// For templated functions
string make_string(string input);

string to_upper_case(string input);
string to_lower_case(string input);

#endif
