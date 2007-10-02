/***************************************************************************
 *
 *   Copyright (C) 2002, 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "CommandParser.h"
#include "strutil.h"

#include <fstream>

using namespace std;

bool CommandParser::debug = false;

CommandParser::CommandParser()
{
  quit = false;
  verbose = false;
  fault = false;
}

CommandParser::~CommandParser()
{
  for (unsigned icmd=0; icmd < commands.size(); icmd++)
    delete commands[icmd];
}

void CommandParser::import (CommandParser* other)
{
  for (unsigned icmd=0; icmd < other->commands.size(); icmd++)
    add_command (other->commands[icmd]);
}

void CommandParser::script (const string& filename)
{
  vector<string> cmds;
  loadlines (filename, cmds);
  script (cmds);
}

void CommandParser::script (const vector<string>& cmds)
{
  fault = false;
  for (unsigned i=0; i<cmds.size(); i++) {
    string response = parse( cmds[i] );
    if (fault)
      throw Error (InvalidState, "CommandParser::script", response);
    cerr << response;
  }
}

static const char* whitespace = " \t\n";

string CommandParser::parse (const string& commandargs)
{
  if (debug)
    cerr << "CommandParser::parse '" << commandargs << "'" << endl;

  string cmdargs = commandargs;
  string command = stringtok (&cmdargs, whitespace);

  if (debug)
    cerr << "CommandParser::parse '"<< command <<"' '"<< cmdargs <<"'"<<endl;

  return parse2 (command, cmdargs);
}

string CommandParser::empty ()
{
  return "";
}

string CommandParser::parse2 (const string& command, const string& arguments)
{
  if (debug)
    cerr << "CommandParser::parse '"<< command <<"' '"<< arguments <<"'"<<endl;

  // nested CommandParsers may support empty command strings
  if (command.empty() && nested.empty())
    return empty();

  if (debug)
    cerr << "CommandParser::parse command not empty" << endl;

  // //////////////////////////////////////////////////////////////////////
  //
  // quit, exit, verbose, help
  //
  if (command == "quit" || command == "exit") {
    quit = true;
    return "\n";
  }

  if (debug)
    cerr << "CommandParser::parse command not quit" << endl;

  if (command == "verbose") {
    verbose = !verbose;
    if (verbose)
      return "verbosity enabled\n";
    else
      return "";
  }

  if (debug)
    cerr << "CommandParser::parse command not verbose" << endl;

  if (command == "help" || command == "?")
    return help (arguments);

  if (debug)
    cerr << "CommandParser::parse command not help" << endl;

  bool shortcut = command.length() == 1;
  unsigned icmd = 0;

  // add new lines to output only if this CommandParser is not nested
  string newline;
  if (nested.empty())
    newline = "\n";

  for (icmd=0; icmd < commands.size(); icmd++)

    if ( (shortcut && command[0] == commands[icmd]->shortcut)
	 || command == commands[icmd]->command) {

      current_command = commands[icmd]->command;

      if (debug)
	cerr << "CommandParser::parse execute " << command << endl;

      string reply = commands[icmd]->execute (arguments);

      if (debug)
	cerr << "CommandParser::parse execute reply '" << reply << "'" << endl;

      if (reply.empty())
	return "";
      else
	return reply + newline;
    }

  // special case: command may be a string of shortcut keys

  unsigned length = command.length();

  if (length == 0)
    return empty();

  unsigned ikey = 0;

  for (ikey=0; ikey < length; ikey++) {
    for (icmd=0; icmd < commands.size(); icmd++)
      if (command[ikey] == commands[icmd]->shortcut)
	break;
    if (icmd == commands.size())
      break;
  }

  if (ikey != length)  {
    fault = true;
    return "invalid command: " + command + newline;
  }

  // otherwise, every character in the command is a shortcut key

  string total_reply;

  for (ikey=0; ikey < length; ikey++)
    for (icmd=0; icmd < commands.size(); icmd++)
      if (command[ikey] == commands[icmd]->shortcut) {

	current_command = commands[icmd]->command;

	// only the last command gets the arguments
	string args = (ikey == length-1) ? arguments : "";
	string reply = commands[icmd]->execute (args);

	if (!reply.empty())
	  total_reply += reply + "\n";

      }

  return total_reply;
}

string CommandParser::help (const string& command)
{
  unsigned icmd = 0;

  if (command.empty()) {

    bool have_shortcuts = false;
    unsigned maxlen = 0;

    /* if the CommandParser is nested, then prefix its name to all
       command names printed in the help */

    string nested_prefix = "";
    if (nested.length())
      nested_prefix = nested + " ";

    /* determine the maximum command string length, 
       and if any shortcut keys are enabled */

    for (icmd=0; icmd < commands.size(); icmd++) {
      string cmd = nested_prefix + commands[icmd]->command;
      if (cmd.length() > maxlen)
	maxlen = cmd.length();
      if (commands[icmd]->shortcut)
	have_shortcuts = true;
    }

    /* include a space after the command column */
    if (have_shortcuts)
      maxlen ++;
    else
      maxlen += 3;  // looks better with a bit more space

    string help_str;

    if (nested.empty())
      help_str = "Available commands";
    else
      help_str = "usage";

    if (have_shortcuts)
      help_str += " (shortcut keys in [] brackets)";

    help_str += ":\n";

    if (nested.empty())
      help_str += "\n";

    for (icmd=0; icmd < commands.size(); icmd++) {

      /* pad command strings with spaces on the right, up to the maximum
	 command string length (plus one space) */
      help_str += pad(maxlen, nested_prefix + commands[icmd]->command);

      /* if any shortcut keys are enabled, print these */
      if (have_shortcuts) {
	if (commands[icmd]->shortcut)
	  help_str += string("[") + commands[icmd]->shortcut + "] ";
	else
	  help_str += "    ";
      }

      /* then print the help string */
      help_str += commands[icmd]->help + "\n";
    }

    /* no shortcut keys used for the built-in commands */
    if (have_shortcuts)
      maxlen += 4;

    if (nested.empty())
      help_str += "\n" +
	pad(maxlen, "quit")    + "quit program\n" +
	pad(maxlen, "verbose") + "toggle verbosity\n";

    help_str += "\n"
      "Type \"" + nested_prefix + "help <command>\" "
      "to get detailed help on each command\n";

    return help_str;
  }

  // a command was specified

  if (command == "quit")
    return "quit|exit exits the program\n";

  if (command == "verbose")
    return "verbose makes the program more verbose\n";

  for (icmd=0; icmd < commands.size(); icmd++)
    if (command == commands[icmd]->command) {
      string help_str = command +": "+ commands[icmd]->help +"\n\n";
      if (commands[icmd]->detail().empty())
	return help_str + "\t no detailed help available\n";
      else
	return help_str + commands[icmd]->detail() + "\n";
    }
      
  return "invalid command: " + command + "\n";
}

void CommandParser::add_command (Method* command)
{
  for (unsigned icmd=0; icmd < commands.size(); icmd++) {

    if (command->command == commands[icmd]->command)
      throw Error (InvalidParam, "CommandParser::add_command",
		   "command name='" + command->command + "' already taken");

    if (command->shortcut && command->shortcut == commands[icmd]->shortcut)
      throw Error (InvalidParam, "CommandParser::add_command",
		   "command shortcut=%c already taken", command->shortcut);

  }

  commands.push_back (command);
}

CommandParser::Nested::Nested( CommandParser* _parser,
			       const string& _command,
			       const string& _help,
			       char _shortcut )
{
  command  = _command;
  help     = _help;
  shortcut = _shortcut;

  parser   = _parser;

  parser->nested = command;
}

string CommandParser::Nested::execute (const string& command)
{
  return parser->parse(command);
}

string CommandParser::Nested::detail () const
{
  return parser->help();
}

void CommandParser::import (CommandParser* parser,
			    const string& command,
			    const string& help,
			    char shortcut)
{
  add_command( new Nested(parser, command, help, shortcut) );
}
