//-*-C++-*-
/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Attic/string_utils.h,v $
   $Revision: 1.32 $
   $Date: 2004/10/13 14:10:58 $
   $Author: straten $ */

#ifndef __STRING_UTILS_H
#define __STRING_UTILS_H

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <iomanip>
#include <sstream>
//#include <fstream>
//#include <ostream>
#include <string>
#include <vector>

#include "psr_cpp.h"

// These return the input string
string& chop(string& ss);
string& chop(string& ss, unsigned chars);

string& chomp(string& ss, char gone='\n');
string& chomp(string& ss, string gone);

string& frontchop(string& ss);
string& frontchop(string& ss, unsigned chars);

string& frontchomp(string& ss, char gone);
string& frontchomp(string& ss, string gone);

// Removes 1 instance from anywhere in the string- really should be called 'replace_first_of'
string& midchomp(string& ss, string gone);

// These return true on success
bool h_chop(string& ss);
bool h_chop(string& ss, unsigned chars);

bool h_chomp(string& ss, char gone='\n');
bool h_chomp(string& ss, string gone);

bool h_frontchop(string& ss);
bool h_frontchop(string& ss, unsigned chars);

bool h_frontchomp(string& ss, char gone);
bool h_frontchomp(string& ss, string gone);

bool h_midchomp(string& ss, string gone);

// These take off or add a leading or trailing word
string pop_word(string& line);
string& push_word(string& line,string word);
string frontpop_word(string& line);
string& frontpush_word(string& line,string word);

// Takes off a leading 'J' or a leading 'B'
string no_JB(string pulsar);

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

// Returns the number of words in a line
unsigned nwords(string line);

// Returns a particular word in a line (0 = first word)
string read_word(string line,unsigned iword);

// Stuff to turn an array of char *'s into a vector of strings
// useful for taking lists of files on the command line
vector<string> cstrarray2vec(const char **vals, int nelem);

// Replaces 'bad' with 'good' for each occurence of 'bad'
// Returns the modified string
string replace_char(string ss,char bad,char good);

template<class T>
string form_string(T input){
  ostringstream ost;
  ost << input;
  return ost.str();
}

template<class T>
string form_string(T input, int precision){
  ostringstream ost;
  ost << setprecision( precision ) << input;
  return ost.str();
}

/* Doesn't work for pointers!  (Stupid linux compiler) */
template<class T>
T convert_string(string ss){
  stringstream iost;
  iost << ss;

  T outie;
  iost >> outie;

  return outie;
}

/* Doesn't work for pointers!  (Stupid linux compiler) */
template<class T>
T convert_string(string ss, int precision){
  stringstream iost;
  iost << setiosflags(ios::fixed) << setprecision( precision ) << ss;

  iost >> setiosflags(ios::fixed) >> setiosflags(ios::showpoint) >> setprecision( precision );

  T outie;
  iost >> setw( precision ) >> outie; 
  
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

// Checks that all characters are digits
bool is_unsigned_integer(const string ss);
// Checks that all characters are digits, except the first one, which may be a '+' or a '-'
bool is_signed_integer(const string ss);

//! a simple command for replacing the extension on a filename
string replace_extension (string filename, const string& extension);

#endif

