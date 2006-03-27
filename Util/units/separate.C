/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "separate.h"
using namespace std;

void separate (string s, vector<string>& c)
{
  bool edit;
  separate (s, c, edit);
}

void separate (string s, vector<string>& c, char lim)
{
  bool edit;
  separate (s, c, edit, lim);
}

void separate (string s, vector<string>& c, bool& edit)
{
  separate (s, c, edit, ',');
}

void separate (string s, vector<string>& commands, bool& edit, char lim)
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
