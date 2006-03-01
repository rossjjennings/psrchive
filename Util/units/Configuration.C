#include "Configuration.h"
#include "stringtok.h"
#include "Error.h"

#include <fstream>

//! Construct from the specified file
Configuration::Configuration (const char* filename)
{
  if (!filename)
    return;

  load (filename);
}

void Configuration::load (const std::string& filename)
{
  std::ifstream input (filename.c_str());
  if (!input)
    throw Error (FailedSys, "Configuration::load", "ifstream("+filename+")");
  
  std::string line;
  
  while (!input.eof()) {

    std::getline (input, line);
    line = stringtok (line, "#\n", false);  // get rid of comments

    if (!line.length())
      continue;

    // parse the key
    std::string key = stringtok (line, " \t");

#ifdef _DEBUG
    std::cerr << "Configuration::load key=" << key << std::endl;
#endif

    if (!line.length())
      continue;

    // parse the equals sign
    std::string equals = stringtok (line, " \t");

    if (equals != "=" || !line.length())
      continue;

#ifdef _DEBUG
    std::cerr << "Configuration::load value=" << line << std::endl;
#endif

    entries.push_back( Entry(key,line) );

  }
 
}

