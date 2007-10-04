/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Configuration.h"
#include "stringtok.h"
#include "Error.h"

#include <fstream>

using namespace std;

//! Construct from the specified file
Configuration::Configuration (const char* filename)
{
  if (!filename)
    return;

  load (filename);
}

void Configuration::load (const string& filename)
{
  ifstream input (filename.c_str());
  if (!input)
    throw Error (FailedSys, "Configuration::load", "ifstream("+filename+")");
  
  string line;
  
  while (!input.eof()) {

    getline (input, line);
    line = stringtok (line, "#\n", false);  // get rid of comments

    if (!line.length())
      continue;

    // parse the key
    string key = stringtok (line, " \t");

#ifdef _DEBUG
    cerr << "Configuration::load key=" << key << endl;
#endif

    if (!line.length())
      continue;

    // parse the equals sign
    string equals = stringtok (line, " \t");

    if (equals != "=" || !line.length())
      continue;

#ifdef _DEBUG
    cerr << "Configuration::load value=" << line << endl;
#endif

    Entry* entry = find (key);
    if (entry) {
#ifdef _DEBUG
      cerr << "Configuration::load over-ride " << key << " = " << line << endl;
#endif
      entry->value = line;
    }

    else {
#ifdef _DEBUG
      cerr << "Configuration::load new " << key << " = " << line << endl;
#endif
      entries.push_back( Entry(key,line) );
    }

  }

  filenames.push_back( filename );
}


Configuration::Entry* Configuration::find (const string& key) const
{
#ifdef _DEBUG
  cerr << "Configuration::find size=" << entries.size();
  cerr << " key=" << key << endl;
#endif

  for (unsigned i=0; i<entries.size(); i++) {
    if (entries[i].key == key) {

#ifdef _DEBUG
      cerr << "Configuration::find value=" << entries[i].value << endl;
#endif
      return const_cast<Entry*>( &(entries[i]) );

    }
  }

#ifdef _DEBUG
  cerr << "Configuration::find NO FIND" << endl;
#endif

  return 0;
}

