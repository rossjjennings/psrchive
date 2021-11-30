/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "strutil.h"
#include "Error.h"
#include <inttypes.h>

#include <algorithm>

#include <ctype.h>
#include <stdlib.h>

using std::vector;
using std::string;

string basename (const string& filename)
{
  string::size_type last = filename.rfind ("/");

  if (last == string::npos)
    return filename;
  else
    return filename.substr (last+1);
}

string pathname (const string& filename)
{
  string::size_type last = filename.rfind ("/");

  if (last == string::npos)
    return string();
  else
    return filename.substr (0, last+1);
}

// redwards 24 Jan 00 
// * stringdecimate -- takes a string of words delimited by characters in
//    delimiters, and pulls out the words as a vector of strings
// * stringdelimit -- does the opposite of the above

vector<string>
stringdecimate(const string& wordstr, string delimiters)
{
  string::size_type pos, end;
  vector <string> words;

  pos = 0;
  bool finished = false;
  do
  {
    pos = wordstr.find_first_not_of(delimiters, pos);
    if (pos == string::npos)
      break;
    end = wordstr.find_first_of(delimiters, pos);
    if (end==string::npos)
    {
      end = wordstr.length();
      finished = true;
    }
    words.push_back(wordstr.substr(pos, end-pos));
    pos = end;
  } while (!finished);

  return words;
}

string remove_all (string input, char c)
{
  unsigned i=0;
  while (i<input.size())
    if (input[i] == c)
      input.erase(i,1);
    else
      i ++;
  return input;
}

void string_split ( string source, string &before, string &after, string delimiter )
{
  string::size_type pos = source.find( delimiter );

  before = "";
  after = "";

  if( pos != string::npos )
  {
    before = source.substr( 0, pos );
    after = source.substr( pos + delimiter.size() );
  }
}


void string_split_on_any ( string source, string &before, string &after, string delim_chars )
{
  string::size_type pos = source.find_first_of( delim_chars );

  before = "";
  after = "";

  if( pos != string::npos )
    {
      before = source.substr( 0, pos );
      after = source.substr( pos + 1 );
    }
}

void string_split_on_any ( const string& source, vector<string>& result, string delim_chars )
{
  string::size_type start = 0;

  while (1)
  {
    string::size_type pos = source.find_first_of( delim_chars, start );
  
    if (pos == string::npos)
      break;

    result.push_back( source.substr( start, pos-start ) );
    start = pos + 1;
  }

  result.push_back( source.substr( start ) );    
}

