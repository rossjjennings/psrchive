#include "environ.h"

#include <string>
#include <vector>
#include <algorithm>

#include <stdio.h>

#include "string_utils.h"

// Like perl chomp
bool h_chomp(string& ss,char gone){
  //fprintf(stderr,"2hiyee\n");
  if(ss[ss.size()-1]==gone){
    ss.resize(ss.size()-1);
    return true;
  }
  return false;
}

bool h_chomp(string& ss,string gone){
  //fprintf(stderr,"ss='%s' and gone='%s'\n",ss.c_str(),gone.c_str());
  //fprintf(stderr,"ss.size='%d' and gone.size='%d'\n",ss.size(),gone.size());
  //fprintf(stderr,"ss.length='%d' and gone.length='%d'\n",ss.length(),gone.length());
  //fprintf(stderr,"ss[%d]=%c\n",ss.size()-gone.size(),ss[ss.size()-gone.size()]);
  if( ss.find( gone, ss.length()-gone.length()-1) != string::npos ){
    //fprintf(stderr,"resizing as ans was %d\n", ss.find( ss, ss.length()-gone.length()-1));
    ss.resize(ss.size()-gone.size());
    return true;
  }
  //fprintf(stderr,"false result so trying at zero\n");
  //if( ss.find( gone, 0) != string::npos ){
    //fprintf(stderr,"resizing as ans was %d\n", ss.find( ss, 0));
  //ss.resize(ss.size()-gone.size());
  // return true;
  //}
  //fprintf(stderr,"false result\n");
  return false;
}

// Like perl chop
void h_chop(string& ss){
  //fprintf(stderr,"2hiyo\n");
  ss.resize(ss.size()-1);
}

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


string bool2string(bool mybool){
  if(mybool)
    return "true";
  return "false";
}

void* string2ptr(string ss){
  void* ptr;
  sscanf(ss.c_str(),"%p",ptr);
  return ptr;
}

string make_string(int input){
  char dummy[50];
  sprintf(dummy,"%d",input);
  return string(dummy);
}

string make_string(unsigned input){
  char dummy[50];
  sprintf(dummy,"%d",input);
  return string(dummy);
}

string make_string(long input){
  char dummy[50];
  sprintf(dummy,"%ld",input);
  return string(dummy);
}

string make_string(unsigned long input){
  char dummy[50];
  sprintf(dummy,"%ld",input);
  return string(dummy);
}

string make_string(long long input){
  char dummy[50];
  sprintf(dummy,"%lld",input);
  return string(dummy);
}

string make_string(unsigned long long input){
  char dummy[50];
  sprintf(dummy,"%lld",input);
  return string(dummy);
}

string make_string(float input){
  char dummy[50];
  sprintf(dummy,"%f",input);
  return string(dummy);
}

string make_string(float input, int decimal_places){
  char dummy1[50];
  char dummy2[50];
  sprintf(dummy1,"%%.%df",decimal_places);
  sprintf(dummy2,dummy1,input);
  return string(dummy2);
}

string make_string(double input){
  char dummy[50];
  sprintf(dummy,"%f",input);
  return string(dummy);
}

string make_string(double input, int decimal_places){
  char dummy1[50];
  char dummy2[50];
  sprintf(dummy1,"%%.%df",decimal_places);
  sprintf(dummy2,dummy1,input);
  return string(dummy2);
}

string make_string(void* input){
  char dummy[50];
  sprintf(dummy,"%p",input);
  return string(dummy);
}

// just incase someone is dumb enough...
string make_string(char input){
  char dummy[50];
  sprintf(dummy,"%c",input);
  return string(dummy);
}

// just incase someone is dumb enough...
string make_string(unsigned char input){
  char dummy[50];
  sprintf(dummy,"%d",input);
  return string(dummy);
}

