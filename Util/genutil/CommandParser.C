#include "CommandParser.h"
#include "string_utils.h"

CommandParser::CommandParser()
{
  quit = false;
  verbose = false;
}

static const char* whitespace = " \t\n";

string CommandParser::parse (const char* commandargs)
{
  string cmdargs = commandargs;
  string command = stringtok (&cmdargs, whitespace);

  if (verbose)
    cerr << "CommandParser::parse"
      " command=" << command << " args=" << cmdargs << endl;

  return parse (command, cmdargs);
}


string CommandParser::parse (const char* cmd, const char* args)
{
  string command = cmd;
  string arguments = args;

  if (command.empty())
    return prompt;

  // //////////////////////////////////////////////////////////////////////
  //
  // quit, exit, verbose, help
  //
  if (command == "quit" || command == "exit") {
    quit = true;
    return "\n";
  }

  if (command == "verbose") {
    verbose = !verbose;
    if (verbose)
      return "verbosity enabled\n" + prompt;
    else
      return prompt;
  }

  if (command == "help")
    return help (arguments);

  for (unsigned icmd=0; icmd < commands.size(); icmd++)
    if (command == commands[icmd].command) {
      string reply = execute (commands[icmd].token, arguments);
      if (reply.empty())
	return prompt;
      else
	return reply + "\n" + prompt;
    }

  return "invalid command: " + command + "\n" + prompt;
}

//! derived types may add commands to the list using this method
void CommandParser::add_command (int token, const char* cmd, const char* help)
{
  for (unsigned icmd=0; icmd < commands.size(); icmd++)
    if (token == commands[icmd].token) {
      string error ("CommandParser::add_command token taken");
      cerr << error << endl;
      throw error;
    }

  commands.push_back (Command(token, cmd, help));
}

string CommandParser::help (const string& command)
{
  string help_str;

  if (command.empty()) {
    string help_str = "Available commands:\n\n";

    for (unsigned icmd=0; icmd < commands.size(); icmd++)
      help_str += commands[icmd].command + "\t " + commands[icmd].help + "\n";

    help_str += "\nquit \t quit program\n";
    help_str += "verbose\t toggle verbosity\n";

    return help_str + "\n" + prompt;
  }

  // a command was specified
  for (unsigned icmd=0; icmd < commands.size(); icmd++)
    if (command == commands[icmd].command) {
      if (commands[icmd].detail.empty())
	return command + "\t no detailed help available\n" + prompt;
      else
	return command + "\t " + commands[icmd].detail + "\n" + prompt;
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
