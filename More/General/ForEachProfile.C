/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ForEachProfile.h"
#include <map>

using namespace std;

void Pulsar::ForEachProfile::parse (vector<string>& tokens)
{
  map<string, string*> keymap;
  keymap["pol"] = &pol;
  keymap["chan"] = &chan;
  keymap["subint"] = &subint;
    
  vector<string>::iterator it = tokens.begin();
  while (it != tokens.end())
  {
    string txt = *it;
    bool match = false;
    
    map<string, string*>::iterator arg = keymap.begin();
    while (arg != keymap.end())
    {
      string key = (*arg).first;

      if (txt.compare(0,key.length(),key) == 0)
      {
	match = true;
	string& value = *((*arg).second);
	value = txt.substr (key.length());

	/*
	cerr << "Pulsar::ForEachProfile::parse match txt='" << txt
	     << "' and key='" << key << "' and value='" << value << "'"
	     << endl;
	*/
      }

      arg++;
    }

    if (match)
      tokens.erase(it);
    else
      it ++;
  }
}

