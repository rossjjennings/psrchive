#include <sys/types.h>
#include "string_utils.h"

string stringtok (string* instr, const string& delimiters, bool skip_leading)
{
  ssize_t first = instr->find_first_not_of (delimiters);
  ssize_t start = (skip_leading) ? first : 0;
  ssize_t last = instr->find_first_of(delimiters, start);

  string retval;
  if (first == string::npos)
    return retval;

  if (last == string::npos)
    last = instr->length();

  if (!skip_leading && last < first)
    return retval;

  retval = instr->substr(first, last-first);
  instr->erase(0, last);

  return retval;
}
 
