
#include "string_utils.h"

#include <string>
#include <vector>
#include <algorithm>

// redwards 24 Jan 00 
// * stringdecimate -- takes a string of words delimited by characters in
//    delimiters, and pulls out the words as a vector of strings
// * stringdelimit -- does the opposite of the above

vector<string>
stringdecimate(const string& wordstr, const string& delimiters)
{
  string::size_type pos, end;
  vector <string> words;

  pos = 0;
  bool finished = false;
  do
  {
    pos = wordstr.find_first_not_of(delimiters, pos);
    if (pos == string::npos)
      break;
    end = wordstr.find_first_of(delimiters, pos);
    if (end==string::npos)
    {
      end = wordstr.length();
      finished = true;
    }
    words.push_back(wordstr.substr(pos, end-pos));
    pos = end;
  } while (!finished);

  return words;
}

string
stringdelimit(const vector<string>& words, char delimiter)
{
  string str;
  vector<string>::const_iterator i;

  for (i=words.begin(); i != words.end(); ++i)
    str += *i + delimiter;

  // remove that final delimiter
  str.resize(str.length()-1);

  return str;
}

// Stuff to turn an array of char *'s into a vector of strings
// useful for taking lists of files on the command line
vector<string> 
cstrarray2vec(const char **vals, int nelem)
{
  vector<string> v;
  int i;

  for (i=0; i < nelem; i++)
    v.push_back(string(vals[i]));

  return v;
}


