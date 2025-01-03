/***************************************************************************
 *
 *   Copyright (C) 2004-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "TextInterface.h"
#include "NestedTextInterface.h"
#include "ModifyRestore.h"
#include "stringtok.h"
#include "pad.h"

// #define _DEBUG
#include "debug.h"

#include <stdio.h>
#include <string.h>

#include <algorithm>

using namespace std;

bool TextInterface::label_elements = false;

bool TextInterface::matches (const string& name,
			     const string& prefix,
			     const Value* value)
{
  DEBUG("TextInterface::matches prefix=" << prefix << " name=" << name);

  string::size_type length = prefix.length();

  if (length)
  {
    if ( name.length() <= length || name[length] != ':' )
    {
      DEBUG(" false (no colon)");
      return false;
    }

    if ( strncmp (name.c_str(), prefix.c_str(), length) != 0 )
    {
      DEBUG(" false (prefix mismatch)");
      return false;
    }

    length ++;
  }

  string remainder = name.substr (length);

  DEBUG("TextInterface::matches nested name=" << value->get_name() << " remain=" << remainder);

  bool result = value->matches (remainder);

  DEBUG("TextInterface::matches result=" << result);
  return result;
}


/*! Parses text into key, range, and remainder
  \retval true if key matches name
*/
bool TextInterface::match (const string& name, const string& text,
			   string* range, string* remainder)
{
  DEBUG("TextInterface::match name=" << name << " text=" << text)

  string::size_type length = name.length();

  if ( length && strncmp (text.c_str(), name.c_str(), length) != 0 )
  {
    DEBUG(" false");
    return false;
  }

  string::size_type end = text.find (':', length);
  if (end == string::npos)
  {
    if (remainder)
    {
      DEBUG(" false");
      return false;
    }
    else
      end = text.length();
  }

  DEBUG(" maybe");

  // the range is everything between the end of the variable name and the colon
  *range = text.substr (length, end-length);

  DEBUG("TextInterface::match range=" << *range);

  // the remainder is everything following the colon
  if (remainder)
  {
    *remainder = text.substr (end+1);
    DEBUG("TextInterface::match remainder=" << *remainder);
  }

  // a map may have no name
  if (!length)
    return true;

  // the range may be a wildcard
  if ((*range) == "*" || (*range) == "?")
    return true;

  length = range->length();

  // a range may be empty ...
  if (!length)
    return true;

  // .. or must be enclosed in square brackets
  if ((*range)[0] == '[' && (*range)[length-1] == ']')
    return true;

  DEBUG("FALSE");

  return false;
}

