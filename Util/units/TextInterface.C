#include "TextInterface.h"
#include "stringtok.h"
#include <stdio.h>

using namespace std;

bool TextInterface::verbose = false;

void TextInterface::parse_indeces (vector<unsigned>& index, string& name)
{
  index.resize (0);

  // look for the opening braces
  if (name[0] != '[')
    return;

  if (verbose)
    cerr << "TextInterface::parse_indeces range started" << endl;

  // look for the closing braces
  string::size_type pos = name.find("]:");

  if (pos == string::npos)
    throw Error (InvalidParam, "TextInterface::parse_indeces",
		 "no end of range in '" + name + "'");

  string range = name.substr (1,pos-1);

  if (verbose)
    cerr << "TextInterface::parse_indeces range=" << range << endl;

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
    int start, end;
    char c;

    int scanned = sscanf (sub.c_str(), "%d%c%d", &start, &c, &end);

    if (scanned == 3)  {

      if (c != '-')
        throw Error (InvalidParam, "TextInterface::parse_indeces",
                     "invalid sub-range '" + range + "'");

      if (verbose)
        cerr << "TextInterface::parse_indeces start=" << start 
             << " end=" << end << endl;

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

      if (verbose)
        cerr << "TextInterface::parse_indeces index=" << start << endl;

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

  if (verbose)
    cerr << "TextInterface::parse_indeces remaining name=" << name << endl;

}



