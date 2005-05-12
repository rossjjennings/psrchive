#include "TextInterface.h"
#include "stringtok.h"

#include <stdio.h>

using namespace std;

bool TextInterface::label_elements = false;

void TextInterface::parse_indeces (vector<unsigned>& index, string& name)
{
  index.resize (0);

  // look for the opening braces
  if (name[0] != '[')
    return;

#ifdef _DEBUG
  cerr << "TextInterface::parse_indeces range started" << endl;
#endif

  // look for the closing braces
  string::size_type pos = name.find("]:");

  if (pos == string::npos)
    throw Error (InvalidParam, "TextInterface::parse_indeces",
		 "no end of range in '" + name + "'");

  string range = name.substr (1,pos-1);

#ifdef _DEBUG
  cerr << "TextInterface::parse_indeces range=" << range << endl;
#endif

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

  // satisfied that the range is valid, remove it from the qualified name
  name.erase (0, pos+2);

#ifdef _DEBUG
  cerr << "TextInterface::parse_indeces remaining name=" << name << endl;
#endif

}



