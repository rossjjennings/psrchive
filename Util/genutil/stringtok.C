#include "string_utils.h"

string stringtok (string* instr, const string& delimiters)
{
  ssize_t first = instr->find_first_not_of(delimiters);
  ssize_t last = instr->find_first_of(delimiters, first);

  if (first == instr->npos)
    return string("");
  if (last == instr->npos)
    last = instr->length();

  string retval = instr->substr(first, last-first);
  instr->erase(0,last);

  return retval;
}
