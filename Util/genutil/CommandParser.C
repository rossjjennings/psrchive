#include "CommandParser.h"
#include "string_utils.h"

bool CommandParser::debug = false;

CommandParser::CommandParser()
{
  quit = false;
  verbose = false;
  current_command = 0;
}

static const char* whitespace = " \t\n";

string CommandParser::parse (const char* commandargs)
{
  if (debug)
  cerr << "CommandParser::parse '" << commandargs << "'" << endl;


  string cmdargs = commandargs;
  string command = stringtok (&cmdargs, whitespace);

  if (debug)
  cerr << "CommandParser::parse '"<< command <<"' '"<< cmdargs <<"'"<<endl;


  return parse (command, cmdargs);
}


string CommandParser::parse (const char* cmd, const char* args)
{
  string command = cmd;
  string arguments = args;

  if (debug)
  cerr << "CommandParser::parse '"<< command <<"' '"<< arguments <<"'"<<endl;


  if (command.empty())
    return prompt;

  if (debug)
  cerr << "CommandParser::parse command not empty"<<endl;


  // //////////////////////////////////////////////////////////////////////
  //
  // quit, exit, verbose, help
  //
  if (command == "quit" || command == "exit") {
    quit = true;
    return "\n";
  }

  if (debug)
  cerr << "CommandParser::parse command not quit"<<endl;


  if (command == "verbose") {
    verbose = !verbose;
    if (verbose)
      return "verbosity enabled\n" + prompt;
    else
      return prompt;
  }

  if (debug)
  cerr << "CommandParser::parse command not verbose"<<endl;


  if (command == "help")
    return help (arguments);

  if (debug)
    cerr << "CommandParser::parse command not help"<<endl;


  for (unsigned icmd=0; icmd < commands.size(); icmd++)
    if (command == commands[icmd].command) {

      current_command = icmd;

      if (debug)
	cerr << "CommandParser::parse execute " << command <<endl;


      string reply = execute (commands[icmd].token, arguments);

      if (debug)
	cerr << "CommandParser::parse execute returns '" << reply <<"'"<<endl;


      if (reply.empty())
	return prompt;
      else
	return reply + "\n" + prompt;
    }

  return "invalid command: " + command + "\n" + prompt;
}

//! derived types may add commands to the list using this method
void CommandParser::add_command (int token, const char* cmd, 
				 const char* help, const char* detailed_help)
{
  for (unsigned icmd=0; icmd < commands.size(); icmd++)
    if (token == commands[icmd].token) {
      string error ("CommandParser::add_command token taken");
      cerr << error << endl;
      throw error;
    }

  commands.push_back (Command(token, cmd, help, detailed_help));
}

string CommandParser::usage ()
{
  return "usage: " + commands[current_command].detail;
}

string CommandParser::help (const string& command)
{
  if (command.empty()) {
    string help_str = "Available commands:\n\n";

    for (unsigned icmd=0; icmd < commands.size(); icmd++)
      help_str += commands[icmd].command + "\t " + commands[icmd].help + "\n";

    help_str += 
      "\nquit \t quit program\n"
      "verbose\t toggle verbosity\n\n"
      "Type \"help command\" to get detailed help on each command\n";

    return help_str + "\n" + prompt;
  }

  // a command was specified

  if (command == "quit")
    return "quit|exit exits the program\n" + prompt;

  if (command == "verbose")
    return "verbose makes the program more verbose\n" + prompt;

  for (unsigned icmd=0; icmd < commands.size(); icmd++)
    if (command == commands[icmd].command) {
      string help_str = command +"\t "+ commands[icmd].help +"\n\n";
      if (commands[icmd].detail.empty())
	return help_str + "\t no detailed help available\n" + prompt;
      else
	return help_str + commands[icmd].detail + "\n" + prompt;
    }
      
  return "invalid command: " + command + "\n" + prompt;
}


Command::Command (int _token, const char* _command,
		  const char* _help, const char* _detail)
{
  token   = _token;
  command = _command;
  help    = _help;
  if (_detail)
    detail = _detail;
}
