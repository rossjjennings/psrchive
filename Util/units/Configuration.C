#include "Configuration.h"
#include "stringtok.h"
#include "Error.h"

#include <fstream>

//! Construct from the specified file
Configuration::Configuration (const char* filename)
{
  if (!filename)
    return;

  std::ifstream input (filename);
  if (!input)
    throw Error (FailedSys, "Configuration::load", "ifstream (%s)", filename);

  std::string line;
  
  while (!input.eof()) {

    std::getline (input, line);
    line = stringtok (line, "#\n", false);  // get rid of comments

    if (!line.length())
      continue;

    // parse the key
    std::string key = stringtok (line, " \t");

    if (!line.length())
      continue;

    // parse the equals sign
    std::string equals = stringtok (line, " \t");

    if (equals != "=" || !line.length())
      continue;

    entries.push_back( Entry(key,line) );

  }
 
}
