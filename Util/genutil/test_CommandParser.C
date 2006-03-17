/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <iostream>

#include "CommandParser.h"

class parent : public CommandParser {

public:
  parent () {
    prompt = "parent> ";
    add_command (&parent::testing, "test", "this is a test command parser");
    add_command (&parent::trying, "try",  "type the commands listed");
    add_command (&parent::erring, "error","or type a command not listed");
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
