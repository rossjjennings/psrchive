#include <iostream>

#include "CommandParser.h"

class tester : public CommandParser {

public:
  tester () {
    prompt = "tester> ";
    add_command (&tester::testing, "test", "this is a test command parser");
    add_command (&tester::trying, "try",  "type the commands listed");
    add_command (&tester::erring, "error","or type a command not listed");
  }

  string testing (const string& args) { return "testing"; }
  string trying (const string& args) { return ""; }
  string erring (const string& args) { return "no error here"; }

};

int main (int argc, char** argv)
{
  tester processor;

  cout << processor.prompt;

  while (!processor.quit)  {
    string command;
    getline (cin, command);
    cout << processor.parse (command);
  }

  return 0;
}
