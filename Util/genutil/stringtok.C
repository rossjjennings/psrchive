#include <sys/types.h>
#include "string_utils.h"

string stringtok (string* instr, const string& delimiters)
{
  ssize_t first = instr->find_first_not_of(delimiters);
  ssize_t last = instr->find_first_of(delimiters, first);

  string retval;
  if (first == string::npos)
    return retval;

  if (last == string::npos)
    last = instr->length();

  retval = instr->substr(first, last-first);
  instr->erase(0, last);

  return retval;
}
 
