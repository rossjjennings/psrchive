#include <iostream>

#include "CommandParser.h"

class tester : public CommandParser {

public:
  tester () {
    prompt = "tester> ";
    add_command (0, "test", "this is a test command parser");
    add_command (1, "try",  "try it out by typing the commands listed");
    add_command (2, "error","or by typing a command not listed");
  }

  string execute (int token, const string& args) {
    switch (token) {
    case 0:
      return "testing";
    case 1:
      return "";
    case 2:
      return "no error";
    }
  }
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
