#include "environ.h"

#include <string>
#include <vector>
#include <algorithm>

#include <stdio.h>
#include <stdlib.h>

#include "string_utils.h"

// Like perl chomp
bool h_chomp(string& ss,char gone){
  if(ss[ss.size()-1]==gone){
    ss.resize(ss.size()-1);
    return true;
  }
  return false;
}

bool h_chomp(string& ss,string gone){
  if( ss.find( gone, ss.length()-gone.length()) != string::npos ){
    ss.resize(ss.size()-gone.size());
    return true;
  }
  return false;
}

// Like perl chop
string& h_chop(string& ss){
  ss.resize(ss.size()-1);
  return ss;
}
// Exactly the same as h_chop()
string& chop(string& ss){
  ss.resize(ss.size()-1);
  return ss;
}

string& chomp(string& ss,char gone){
  if(ss[ss.size()-1]==gone)
    ss.resize(ss.size()-1);
  return ss;
}

string& chomp(string& ss,string gone){
  if( ss.find( gone, ss.length()-gone.length()) != string::npos )
    ss.resize(ss.size()-gone.size());
  return ss;
}

// Like fscanf(fptr,"%s%s",ignore,answer) except it gets more than 1 word
bool retrieve_cstring(FILE* fptr,string ignore,char* answer){
  char dummy[1024];
  fgets(dummy,1023,fptr);
  //  fprintf(stderr,"\n\nretrieve_cstring() got dummy='%s'\n",dummy);
  string ss(dummy);
  h_chomp(ss);
  ss.replace(0,ignore.length(),"");
  //fprintf(stderr,"retrieve_cstring() got ss='%s'\n",ss.c_str());
  strcpy(answer,ss.c_str());
  //fprintf(stderr,"retrieve_cstring() got answer='%s'\n",answer);
  return true;
}

// redwards 24 Jan 00 
// * stringdecimate -- takes a string of words delimited by characters in
//    delimiters, and pulls out the words as a vector of strings
// * stringdelimit -- does the opposite of the above

vector<string>
stringdecimate(const string& wordstr, string delimiters)
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
  sscanf(ss.c_str(),"%p",&ptr);
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

string to_upper_case(string input){
  string out = input;
  for(unsigned int i=0; i < input.size(); i++)
    out[i] = toupper(input[i]);
  return out;
}

string to_lower_case(string input){
  string out = input;
  for(unsigned int i=0; i < input.size(); i++)
    out[i] = tolower(input[i]);
  return out;
}
