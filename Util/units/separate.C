/***************************************************************************
 *
 *   Copyright (C) 2004 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// #define _DEBUG 1

#include "separate.h"
#include "Separator.h"

using namespace std;


bool braced (const std::string& txt)
{
  if (txt.length() < 2)
    return false;

  char first = *(txt.begin());
  char last = *(txt.end()-1);

  return (first == '(' && last == ')') || (first == '{' && last == '}');
}


void separate (string s, vector<string>& commands, const string& delimiters)
{
  Separator separator;
  separator.set_delimiters (delimiters);
  separator.separate (s, commands);
}

void standard_separation (vector<string>& list, const string& str)
{
  if (str.find (';') != string::npos)
    separate (str, list, ";");
  else
    separate (str, list, ",");
}

