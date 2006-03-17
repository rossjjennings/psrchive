/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifndef __CommandParser_h
#define __CommandParser_h

#include <vector>
#include <string>
#include <iostream>

#include "psr_cpp.h"
#include "ReferenceAble.h"

class ParseMethod;

class CommandParser : public Reference::Able {

 public:
  //! verbose output from CommandParser methods
  static bool debug;

  //! the prompt shown to the user
  string prompt;

  //! null constructor
  CommandParser ();

  //! destructor
  virtual ~CommandParser () {}

  //! Initialize GNU readline and enable command completion
  void initialize_readline (const char*);

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

  string usage ();

 protected:
  //! Sub-classes add commands to the list using this method
  template <class Parser>
    void add_command (string (Parser::*method)(const string&),
		      const char* command,
		      const char* help,
		      const char* detailed_help = 0);

 private:

  vector<ParseMethod*> commands;

  //! the command index, used by usage()
  unsigned current_command;
  
  static char** completion (const char *text, int start, int end);
  
  static char* command_generator (const char* text, int state);

};

//! Pure virtual base class of the template class Command
class ParseMethod {
 public:
  ParseMethod() {}
  virtual ~ParseMethod () {}
  virtual string execute (const string& command) = 0;

  //! The command string corresponding to this method
  string command;
  //! The help string for this method
  string help;
  //! The detailed help string for this method
  string detail;
};

//! Stores a pointer to a CommandParser sub-class and one of its methods
template <class Parser> class Command : public ParseMethod 
{
  friend class CommandParser;
  
  typedef string (Parser::*Method) (const string&);
  
 public:
  
  Command (Parser* _instance, Method _method, const char* _command,
	   const char* _help, const char* _detailed_help = 0)
    {
      instance = _instance;
      method = _method;
      command = _command;
      help    = _help;
      if (_detailed_help)
	detail = _detailed_help;
    }

  //! Execute method
  string execute (const string& args)
    {
      return (instance->*method) (args);
    }

 protected:
  //! Method of the sub-class to execute
  Method method;

  //! Instance through which method is called
  Parser* instance;

};

//! derived types may add commands to the list using this method
template<class Parser>
void CommandParser::add_command (string (Parser::*method) (const string&),
				 const char* cmd, const char* help, 
				 const char* detailed_help)
{
  if (debug)
    cerr << "CommandParser::add_command \"" << cmd << "\"" << endl;

  Parser* instance = static_cast<Parser*> (this);

#if __GNU_C_PLUS_PLUS_dynamic_cast_BUG_GETS_FIXED
  // this is actually more correct
  Parser* instance = dynamic_cast<Parser*> (this);
  if (!instance) {
    string error ("CommandParser::add_command instance/method mis-match");
    cerr << error << endl;
    throw error;
  }

  if (debug)
    cerr << "CommandParser::add_command dynamic_cast ok" << endl;
#endif

  for (unsigned icmd=0; icmd < commands.size(); icmd++)
    if (cmd == commands[icmd]->command) {
      string error ("CommandParser::add_command command key taken");
      cerr << error << endl;
      throw error;
    }
  
  if (debug)
    cerr << "CommandParser::add_command push_back new Command<Parser>" << endl;

  commands.push_back ( new Command<Parser> (instance, method,
					    cmd, help, detailed_help));
}


#endif
