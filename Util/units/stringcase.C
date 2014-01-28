/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "stringcase.h"

using std::string;

//! Makes the string lowercase
string lowercase (string s)
{
  for( unsigned i=0; i<s.size(); i++)
    s[i] = tolower(s[i]);
  return s;
}

//! Makes the string uppercase
string uppercase (string s)
{
  for( unsigned i=0; i<s.size(); i++)
    s[i] = toupper(s[i]);
  return s;
}

bool c_casecmp (string::value_type l1, string::value_type r1)
{
  return toupper(l1) == toupper(r1);
}

bool casecmp (const string& l, const string& r)
{
  return l.size() == r.size()
    && equal(l.begin(), l.end(), r.begin(), c_casecmp);
}
