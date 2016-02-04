/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Separator.h"

using namespace std;

Separator::Separator()
{
  add_bracket ('[', ']');
  add_bracket ('{', '}');
  add_bracket ('(', ')');
  add_bracket ('"', '"');
  add_bracket ('\'', '\'');

  preserve_numeric_ranges = true;
}

void Separator::add_bracket (char open, char close)
{
  opening_brackets.append(1, open);
  brackets[open] = close;
}

bool Separator::isnumeric (char c)
{
  return isdigit(c) || c=='.' || c=='-';
}

bool Separator::part_of_numeric_range (const string& s, string::size_type pos)
{
  if (pos == 0 || pos+1 == s.length())
    return false;

  const string whitespaces (" \t\f\v\n\r");

  /*
    the primary intent of this function is to ignore commas that are
    part of a numeric range (e.g. "3,4,7-8" ... if the delimiting
    character is whitespace, then there is no need for this check
  */

  if (whitespaces.find( s[pos] ) != string::npos)
    return false;

  string::size_type next = s.find_first_not_of (whitespaces, pos+1);
	
  if (next == string::npos || !isnumeric(s[next]))
    return false;

  string::size_type last = s.find_last_not_of (whitespaces, pos-1);

  if (last == string::npos || !isnumeric(s[last]))
    return false;

  return true;
}
