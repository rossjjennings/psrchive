/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "TextInterface.h"
#include "stringtok.h"

#include <stdio.h>

using namespace std;

bool TextInterface::label_elements = false;

string TextInterface::Class::process (const string& command)
{
  string::size_type set = command.find('=');

  // if no equals sign is present, assume that command is get key
  if (set == string::npos)
    return command + "=" + get_value (command);

  // string before the equals sign
  string before = command.substr (0,set);
  // string after the equals sign
  string after = command.substr (set+1);

  // remove any white space from the key
  string param = stringtok (before, " \t");

  set_value (param, after);
  return "";
}

/*! Parses text into key, range, and remainder
  \retval true if key matches name
*/
bool TextInterface::match (const string& name, const string& text,
			   string* range, string* remainder)
{
  string::size_type length = name.length();

  if ( length && text.compare (0, length, name) != 0 )
    return false;

  string::size_type end = text.find (':', length);
  if (end == string::npos)
    return false;

  // the range is everything between the end of the variable name and the colon
  *range = text.substr (length, end-length);

  // the remainder is everything following the colon
  *remainder = text.substr (end+1);

  // a map may have no name
  if (!length)
    return true;

  // the range may be a wildcard
  if ((*range) == "*" || (*range) == "?")
    return true;

  // the range must be enclosed in square brackets
  length = range->length();
  if ((*range)[0] == '[' && (*range)[length-1] == ']')
    return true;

  return false;
}

void TextInterface::parse_indeces (vector<unsigned>& index,
				   const string& name)
{
  index.resize (0);

  string::size_type length = name.length();

  // look for the opening and closing braces
  if (name.length() < 3 || name[0] != '[' || name[length-1] != ']')
    throw Error (InvalidParam, "TextInterface::parse_indeces",
		 "no range in '" + name + "'");

#ifdef _DEBUG
  cerr << "TextInterface::parse_indeces range started" << endl;
#endif

  string range = name.substr (1,length-2);

  if (range.empty())
    throw Error (InvalidParam, "TextInterface::parse_indeces",
		 "no range in '" + name + "'");

  for (unsigned i=0; i<range.size(); i++)
    if (!isdigit(range[i]) && range[i] != '-' && range[i] != ',')
      throw Error (InvalidParam, "TextInterface::parse_indeces",
		   "invalid range '" + range + "'");

  string backup = range;

  while (!range.empty()) {

    string sub = stringtok (range, ",", false, false);

    if (range[0]==',')
      range.erase(0,1);

    int start, end;
    char c;

    int scanned = sscanf (sub.c_str(), "%d%c%d", &start, &c, &end);

    if (scanned == 3)  {

      if (c != '-')
        throw Error (InvalidParam, "TextInterface::parse_indeces",
                     "invalid sub-range '" + range + "'");

#ifdef _DEBUG
      cerr << "TextInterface::parse_indeces start=" << start 
	   << " end=" << end << endl;
#endif

      if (start < 0)
        throw Error (InvalidRange, "TextInterface::parse_indeces",
                     "start=%d < 0", start);

      if (end < 0)
        throw Error (InvalidRange, "TextInterface::parse_indeces",
                     "end index=%d < 0", end);

      if (end < start)
        throw Error (InvalidRange, "TextInterface::parse_indeces",
                     "end=%d < start=%d", end, start);


      for (int i=start; i <= end; i++)
	index.push_back (i);

    }

    else if (scanned == 1)  {

#ifdef _DEBUG
      cerr << "TextInterface::parse_indeces index=" << start << endl;
#endif

      if (start < 0)
        throw Error (InvalidRange, "TextInterface::parse_indeces",
                     "index=%d < 0", start);

      index.push_back (start);

    }

    else
      throw Error (InvalidParam, "TextInterface::parse_indeces",
		                 "invalid sub-range '" + sub + "'");
  }

}

void TextInterface::separate (string s, vector<string>& c)
{
  bool edit;
  separate (s, c, edit);
}

void TextInterface::separate (string s, vector<string>& c, char lim)
{
  bool edit;
  separate (s, c, edit, lim);
}

void TextInterface::separate (string s, vector<string>& c, bool& edit)
{
  separate (s, c, edit, ',');
}

void TextInterface::separate (string s, vector<string>& commands,
			      bool& edit, char lim)
{
  string limiter;
  limiter += lim;

  while (s.length()) {

    // search for the first instance of lim not enclosed in [ brackets ]
    string::size_type end = 0;

    while ( (end = s.find_first_of (limiter+"[(", end) ) != string::npos ) {
      if (s[end] == '[')
	end = s.find (']', end);
      else if (s[end] == '(')
	end = s.find (')', end);
      else
	break;
    }

    // the first naked comma
    string command = s.substr (0, end);

    if (command.find('='))
      edit = true;

    commands.push_back (command);

    end = s.find_first_not_of (limiter+" ", end);
    s.erase (0, end);
    
  }
}
