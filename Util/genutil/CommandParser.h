#ifndef __CommandParser_h
#define __CommandParser_h

#include <vector>
#include <string>

class Command {

 public:
  Command (int token, const char* command,
	   const char* help, const char* detailed_help = 0);

  int    token;
  string command;
  string help;
  string detail;

};

class CommandParser {

 public:

  //! the prompt shown to the user
  string prompt;

  //! null constructor
  CommandParser ();

  //! destructor
  virtual ~CommandParser () {}

  //! derived types must define what happens for each command parsed
  virtual string execute (int token, const string& args) = 0;

  //! derived types may add commands to the list using this method
  void add_command (int token, const char* command,
		    const char* help, const char* detailed_help);

  //! return a help string
  string help (const string& command);

  //! parse and execute a command with arguments in one string
  string parse (const char* commandargs);

  //! convenience interface
  string parse (const string& commandargs)
    { return parse (commandargs.c_str()); }

  //! parse and execute a command with arguments
  string parse (const char* command, const char* args);

  //! convenience interface
  string parse (const string& command, const char* args)
    { return parse (command.c_str(), args); }

  //! convenience interface
  string parse (const char* command, const string& args)
    { return parse (command, args.c_str()); }

  //! convenience interface
  string parse (const string& command, const string& args)
    { return parse (command.c_str(), args.c_str()); }

  //! quit flag raised by "quit"
  bool quit;

  //! verbose operation enabled by "verbose"
  bool verbose;

 private:

  vector<Command> commands;
  
};


#endif
