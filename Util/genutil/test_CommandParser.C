/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "CommandParser.h"

#include <iostream>

using namespace std;

class parent : public CommandParser {

public:
  parent () {
    prompt = "parent> ";
    add_command (&parent::testing, 'T', "test", "test of command parser");
    add_command (&parent::trying,  "nothing",   "type the commands listed");
    add_command (&parent::erring,  "error",     "or one not listed");
  }

  string testing (const string& args) { return "testing"; }
  string trying (const string& args) { return ""; }
  string erring (const string& args) { return "no error here"; }

};

class child : public parent {

public:
  child () { value = 0; }
  int value;
};

int main (int argc, char** argv)
{
  CommandParser::debug = true;

  cerr << "Creating parent instance" << endl;
  parent test;

  cerr << "Creating child instance" << endl;
  child processor;

  cout << processor.prompt;

  while (!processor.quit)  {
    string command;
    getline (cin, command);
    cout << processor.parse (command);
  }

  return 0;
}
