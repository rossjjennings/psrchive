//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Attic/string_utils.h,v $
   $Revision: 1.42 $
   $Date: 2006/08/28 02:44:39 $
   $Author: hknight $ */

#ifndef __STRING_UTILS_H
#define __STRING_UTILS_H

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "pad.h"

//! Makes the string lowercase
std::string lowercase(std::string s);
//! Makes the string uppercase
std::string uppercase(std::string s);

// These return the input std::string
std::string& chop(std::string& ss);
std::string& chop(std::string& ss, unsigned chars);

std::string& chomp(std::string& ss, char gone='\n');
std::string& chomp(std::string& ss, std::string gone);

std::string& frontchop(std::string& ss);
std::string& frontchop(std::string& ss, unsigned chars);

std::string& frontchomp(std::string& ss, char gone);
std::string& frontchomp(std::string& ss, std::string gone);

// Removes 1 instance from anywhere in the std::string- really should be called 'replace_first_of'
std::string& midchomp(std::string& ss, std::string gone);

// These return true on success
bool h_chop(std::string& ss);
bool h_chop(std::string& ss, unsigned chars);

bool h_chomp(std::string& ss, char gone='\n');
bool h_chomp(std::string& ss, std::string gone);

bool h_frontchop(std::string& ss);
bool h_frontchop(std::string& ss, unsigned chars);

bool h_frontchomp(std::string& ss, char gone);
bool h_frontchomp(std::string& ss, std::string gone);

bool h_midchomp(std::string& ss, std::string gone);

// These take off or add a leading or trailing word
std::string pop_word(std::string& line);
std::string& push_word(std::string& line,std::string word);
std::string frontpop_word(std::string& line);
std::string& frontpush_word(std::string& line,std::string word);

// Takes off a leading 'J' or a leading 'B'
std::string no_JB(std::string pulsar);

// Like fscanf(fptr,"%s%s",ignore,answer) except it gets more than 1 word
bool retrieve_cstring(FILE* fptr,std::string ignore,char* answer);

// ///////////////////////////////////////////////////////////
// reads up to 'nbytes' (or to eof) from 'istr', adding them 
// to 'str'.  returns number of bytes read or -1 on error
// ///////////////////////////////////////////////////////////
ssize_t stringload (std::string* str, std::istream &istr, std::streamsize nbytes=0);

// ///////////////////////////////////////////////////////////
// reads up to 'nbytes' (or to eof) from 'fptr', adding them 
// to 'str'.  returns number bytes read or -1 on error
// ///////////////////////////////////////////////////////////
ssize_t stringload (std::string* str, FILE* fptr, size_t nbytes=0);

// ///////////////////////////////////////////////////////////
// fills a vector of std::string with the first word from each line
// in the file.  A line is delimited by \n or commented by #.
// ///////////////////////////////////////////////////////////
int stringfload (std::vector<std::string>* lines, const char* filename);
int stringload (std::vector<std::string>* lines, FILE* fptr);

// ///////////////////////////////////////////////////////////
// fills a vector of std::string with the lines form file.  
// A line is delimited by \n or commented by #.
// ///////////////////////////////////////////////////////////
void loadlines (const std::string& filename, std::vector<std::string>& lines);

// ///////////////////////////////////////////////////////////
// returns the first sub-std::string of 'instr' delimited by
// characters in 'delimiters'.  the substd::string and any leading
// delimiter characters are removed from 'instr'
// ///////////////////////////////////////////////////////////
std::string stringtok (std::string * instr, const std::string & delimiters,
		       bool skip_leading_delimiters = true,
		       bool strip_leading_delimiters_from_remainder = true);

inline
std::string stringtok (std::string* instr, char delimiter,
		       bool skip_leading_delimiters = true,
		       bool strip_leading_delimiters_from_remainder = true)
{ return stringtok (instr, std::string(1, delimiter),
		    skip_leading_delimiters,
		    strip_leading_delimiters_from_remainder); }

// ///////////////////////////////////////////////////////////
// returns the first sub-std::string of 'instr' delimited by
// 'delimiter'.
// ///////////////////////////////////////////////////////////
std::string subdlim (const std::string& instr, const std::string& delimiter);

inline std::string subdlim (const std::string& instr, char* delimiters)
{ return subdlim (instr, std::string(delimiters)); }

inline std::string subdlim (const std::string& instr, char delimiter)
{ return subdlim (instr, std::string(1, delimiter)); }

// ///////////////////////////////////////////////////////////
int stringlen (double val, unsigned precision = 15);

inline int stringlen (int val, unsigned precision = 0)
{ return stringlen (double(val), precision); }

inline int stringlen (float val, unsigned precision = 6)
{ return stringlen (double(val), precision); }

std::string stringprintf(char *fmt ...);

// Stuff to delimit a vector of std::strings and the reverse
std::vector<std::string> 
stringdecimate(const std::string& wordstr, std::string delimiters);
std::string stringdelimit(const std::vector<std::string>& words, char delimiter);

// Returns one line per element of return vector
std::vector<std::string> stringlines(const std::string& str);

// Returns the number of words in a line
unsigned nwords(std::string line);

// Returns a particular word in a line (0 = first word)
std::string read_word(std::string line,unsigned iword);

// Stuff to turn an array of char *'s into a vector of std::strings
// useful for taking lists of files on the command line
std::vector<std::string> cstrarray2vec(const char **vals, int nelem);

// Replaces 'bad' with 'good' for each occurence of 'bad'
// Returns the modified std::string
std::string replace_char(std::string ss,char bad,char good);

template<class T>
std::string form_string(T input){
  std::ostringstream ost;
  ost << input;
  return ost.str();
}

template<class T>
std::string form_string(T input, int precision){
  std::ostringstream ost;
  ost << std::setprecision( precision ) << input;
  return ost.str();
}

// forward declaration of template method defined
// This is designed to take an integer as input
// Makes 1234567 -> "1,234,567"
template<class T>
std::string
comma_string(T input)
{
  std::string in = form_string(input);

  std::vector<char> vout;
  for( unsigned i=0; i<in.size(); i++){
    unsigned index = in.size()-1-i;
    vout.push_back( in[index] );
    if( i % 3 == 2 )
      vout.push_back( ',' );
  }

  std::string out;
  for( unsigned i=0; i<vout.size(); i++){
    unsigned index = in.size()-1-i;
    out.push_back( vout[index] );
  }

  frontchomp( out, ',' );

  return out;
}

/* Doesn't work for pointers!  (Stupid linux compiler) */
template<class T>
T convert_string(std::string ss){
  std::stringstream iost;
  iost << ss;

  T outie;
  iost >> outie;

  return outie;
}

/* Doesn't work for pointers!  (Stupid linux compiler) */
template<class T>
T convert_string(std::string ss, int precision){
  std::stringstream iost;
  iost << std::setiosflags(std::ios::fixed)
       << std::setprecision( precision ) << ss;

  iost >> std::setiosflags(std::ios::fixed)
       >> std::setiosflags(std::ios::showpoint)
       >> std::setprecision( precision );

  T outie;
  iost >> std::setw( precision ) >> outie; 
  
  return outie;
}

std::string bool2string(bool mybool);
void* string2ptr(std::string ss);

/* these use stdio- which doesn't use costly dynamic memory allocations */
std::string make_string(int input);
std::string make_string(int input, int places);
std::string make_string(unsigned input);
std::string make_string(unsigned input, int places);
std::string make_string(long input);
std::string make_string(unsigned long input);
std::string make_string(long long input);
std::string make_string(unsigned long long input);
std::string make_string(float input);
std::string make_string(float input, int decimal_places);
std::string make_string(double input);
std::string make_string(double input, int decimal_places);
std::string make_string(void* input);
// just incase someone is dumb enough...
std::string make_string(char input);
// just incase someone is CRAZY enough - writes as an int!
std::string make_string(unsigned char input);
// For templated functions
std::string make_string(std::string input);

std::string to_upper_case(std::string input);
std::string to_lower_case(std::string input);

// Checks that all characters are digits
bool is_unsigned_integer(const std::string ss);
// Checks that all characters are digits, except the first one, which may be a '+' or a '-'
bool is_signed_integer(const std::string ss);

//! a simple command for replacing the extension on a filename
std::string replace_extension (std::string filename, const std::string& extension);

//! a simple command for returning a user friendly time std::string
std::string time_string (double seconds);

#endif

