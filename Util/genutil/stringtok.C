/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <sys/types.h>

#include "string_utils.h"
using namespace std;

string stringtok (string* instr, const string& delimiters,
		  bool skip_leading, bool strip_leading_remain)
{
  ssize_t first = instr->find_first_not_of (delimiters);
  ssize_t start = (skip_leading) ? first : 0;
  ssize_t last = instr->find_first_of(delimiters, start);
  if (last == string::npos)
    last = instr->length();

  string retval;
  if (first == string::npos) {
    instr->erase();
    return retval;
  }

  if (!skip_leading && last < first)
    return retval;

  retval = instr->substr(first, last-first);

  if (strip_leading_remain) {
    last = instr->find_first_not_of (delimiters, last);
    if (last == string::npos)
      last = instr->length();
  }

  instr->erase(0, last);

  return retval;
}
 
