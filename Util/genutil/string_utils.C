/***************************************************************************
 *
 *   Copyright (C) 2000 by Russell Edwards
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "string_utils.h"
#include "Error.h"
#include "environ.h"

#include <algorithm>

#include <ctype.h>
#include <stdlib.h>

using namespace std;

string pad (unsigned length, string text, bool right)
{
  while (text.length() < length)
    if (right)
      text += " ";
    else
      text = " " + text;

  return text;
}

string& chop(string& ss){
  if( ss.size()!=0 )
    ss.resize(ss.size()-1);
  return ss;
}

string& chop(string& ss, unsigned chars){
  if( ss.size() < chars )
    ss.resize(0);
  else
    ss.resize( ss.size()-chars );

  return ss;
}

string& chomp(string& ss,char gone){
  if(ss.size()!=0 && ss[ss.size()-1]==gone)
    ss.resize(ss.size()-1);
  return ss;
}

string& chomp(string& ss,string gone){
  if( ss.size() < gone.size() )
    return ss;
  if( ss.find( gone, ss.length()-gone.length()) != string::npos )
    ss.resize(ss.size()-gone.size());
  return ss;
}

string& frontchop(string& ss){
  if( ss.size()!=0 )
    ss = string(ss.begin()+1,ss.end());

  return ss;
}

string& frontchop(string& ss, unsigned chars){
  if( ss.size() <= chars )
    ss.resize(0);
  else
    ss = string(ss.begin()+chars,ss.end());

  return ss;
}

string& frontchomp(string& ss, char gone){
  if( ss.size()==0 )
    return ss;
  
  if( ss[0]==gone )
    ss = string(ss.begin()+1,ss.end());

  return ss;
}

string& frontchomp(string& ss, string gone){
  if( ss.size() < gone.size() )
    return ss;

  if( ss.substr(0,gone.size())==gone )
    ss = string(ss.begin()+gone.size(),ss.end());

  return ss;
}

string& midchomp(string& ss, string gone){
  h_midchomp(ss,gone);
  return ss;
}

bool h_chop(string& ss){
  if( ss.size()==0 )
    return false;
  ss.resize(ss.size()-1);
  return true;
}

bool h_chop(string& ss,unsigned chars){
  if( ss.size() < chars ){
    ss.resize(0);
    return false;
  }

  ss.resize(ss.size()-chars);
  return true;
}

bool h_chomp(string& ss,char gone){
  if( ss.size()==0 )
    return false;

  if(ss[ss.size()-1]==gone){
    ss.resize(ss.size()-1);
    return true;
  }
  return false;
}

bool h_chomp(string& ss,string gone){
  if( ss.size() < gone.size() )
    return false;

  if( ss.find( gone, ss.length()-gone.length()) != string::npos ){
    ss.resize(ss.size()-gone.size());
    return true;
  }
  return false;
}

bool h_midchomp(string& ss,string gone){
  if( ss.size() < gone.size() )
    return false;

  string::size_type pos = ss.find(gone,0);

  if( pos == string::npos )
    return false;

  string new_ss = ss.substr(0,pos);

  string::size_type replace_end = pos+gone.size();

  if( replace_end != ss.size() )
    new_ss += ss.substr(replace_end,ss.size()-replace_end);

  ss = new_ss;

  return true;
}

bool h_frontchop(string& ss){
  if( ss.size()==0 )
    return false;
  ss = string(ss.begin()+1,ss.end());
  return true;
}

bool h_frontchop(string& ss, unsigned chars){
  if( ss.size() < chars ){
    ss.resize(0);
    return false;
  }
  ss = string(ss.begin()+chars,ss.end());
  return true;
}

bool h_frontchomp(string& ss, char gone){
  if( ss.size() == 0 )
    return false;

  if( ss[0]==gone ){
    ss = string(ss.begin()+1,ss.end());
    return true;
  }
  return false;
}

bool h_frontchomp(string& ss, string gone){
  if( ss.size() < gone.size() )
    return false;

  if( ss.substr(0,gone.size())==gone ){
    ss = string(ss.begin()+gone.size(),ss.end());
    return true;
  }
  return false;
}

// Pop off a trailing word from the end of the line
string pop_word(string& line){
  vector<string> words = stringdecimate(line," \t");
  string lastword = words.back();
  words.pop_back();
  line = stringdelimit(words,'\t');
  return lastword;
}

// Push on a trailing word onto the end of the line
string& push_word(string& line,string word){
  line += '\t' + word;
  return line;
}

// Pop off a leading word from the start of the line
string frontpop_word(string& line){
  vector<string> words = stringdecimate(line," \t");
  string firstword = words.front();
  words = vector<string>(words.begin()+1,words.end());
  line = stringdelimit(words,'\t');
  return firstword;
}

// Push on a leading word onto the start of the line
string& frontpush_word(string& line,string word){
  line = word + '\t' + line;
  return line;
}

// Takes off a leading 'J' or a leading 'B'
string no_JB(string pulsar){
  frontchomp(pulsar,'J');
  frontchomp(pulsar,'B');
  return pulsar;
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

// Returns one line per element of return vector
vector<string> stringlines(const string& str)
{
  vector<string> lines;

  if( str.size()==0 )
    return lines;
  
  char* cptr = (char*)&*str.begin();
  char* dend = cptr + str.size();
  char* start = cptr;

  while( cptr!=dend ){
    if( *cptr=='\n' ){
      lines.push_back( string(start,cptr) );
      start = cptr+1;
    }
    cptr++;
  }

  if( start!=dend )
    lines.push_back( string(start,dend) );

  return lines;
}

// Returns the number of words in a line
unsigned nwords(string line){
  vector<string> words = stringdecimate(line," \t");
  return words.size();
}

// Returns a particular word in a line (0 = first word)
string read_word(string line,unsigned iword){
  vector<string> words = stringdecimate(line," \t");
  if( words.size() <= iword )
    throw Error(InvalidState,"read_word()",
		"Only %d words in line '%s' and you wanted word %d",
		words.size(), line.c_str(), iword);
  return words[iword];
}

string stringdelimit(const vector<string>& words, char delimiter){
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

string replace_char(string ss,char bad,char good){
  if( ss.size()==0 || bad==good )
    return ss;
  
  string::size_type i=ss.find(bad,0);

  while( i!=string::npos ){
    ss[i] = good;
    i = ss.find(bad,i);
  }

  return ss;
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

string make_string(int input, int places){
  char dummy1[50];
  char dummy2[50];
  sprintf(dummy1,"%%%dd",places);
  sprintf(dummy2,dummy1,input);

  string ss = dummy2;

  return replace_char(ss,' ','0');
}

string make_string(unsigned input){
  char dummy[50];
  sprintf(dummy,"%d",input);
  return string(dummy);
}

string make_string(unsigned input, int places){
  char dummy1[50];
  char dummy2[50];
  sprintf(dummy1,"%%%dd",places);
  sprintf(dummy2,dummy1,input);
  
  string ss = dummy2;

  return replace_char(ss,' ','0');
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

string make_string(string input){
  return input;
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

// Checks that all characters are digits
bool is_unsigned_integer(const string ss){
  if( ss.size()==0 )
    return false;

  for( unsigned i=0; i<ss.size(); i++)
    if( !isdigit(ss[i]) )
      return false;
  return true;
}

// Checks that all characters are digits, except the first one, which may be a '+' or a '-'
bool is_signed_integer(const string ss){
  if( ss.size()==0 )
    return false;

  if( ss.size()==1 )
    return isdigit(ss[0]);

  if( ss[0]=='-' || ss[0]=='+' || isdigit(ss[0]) )
    return is_unsigned_integer(ss.substr(1,ss.size()-1));

  return false;
}
