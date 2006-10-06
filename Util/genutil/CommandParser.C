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

  return parse (command, cmdargs);
}


string CommandParser::parse (const string& command, const string& arguments)
{
  if (debug)
    cerr << "CommandParser::parse '"<< command <<"' '"<< arguments <<"'"<<endl;

  if (command.empty())
    return "";

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
  for (icmd=0; icmd < commands.size(); icmd++)

    if ( (shortcut && command[0] == commands[icmd]->shortcut)
	 || command == commands[icmd]->command) {

      current_command = commands[icmd]->command;

      if (debug)
	cerr << "CommandParser::parse execute " << command << endl;

      string reply = commands[icmd]->execute (arguments);

      if (debug)
	cerr << "CommandParser::parse execute returns '" << reply <<"'"<<endl;

      if (reply.empty())
	return "";
      else
	return reply + "\n";
    }

  // special case: command may be a string of shortcut keys

  unsigned length = command.length();
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
    return "invalid command: " + command + "\n";
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
    string help_str = "Available commands:\n\n";

    unsigned maxlen = 0;
    for (icmd=0; icmd < commands.size(); icmd++)
      if (commands[icmd]->command.length() > maxlen)
	maxlen = commands[icmd]->command.length();

    maxlen += 3;

    for (icmd=0; icmd < commands.size(); icmd++)
      help_str += pad(maxlen, commands[icmd]->command) + commands[icmd]->help
	+ "\n";

    help_str += "\n" 
      + pad(maxlen, "quit")    + "quit program\n"
      + pad(maxlen, "verbose") + "toggle verbosity\n\n"
      "Type \"help command\" to get detailed help on each command\n";

    return help_str + "\n";
  }

  // a command was specified

  if (command == "quit")
    return "quit|exit exits the program\n";

  if (command == "verbose")
    return "verbose makes the program more verbose\n";

  for (icmd=0; icmd < commands.size(); icmd++)
    if (command == commands[icmd]->command) {
      string help_str = command +": "+ commands[icmd]->help +"\n\n";
      if (commands[icmd]->detail.empty())
	return help_str + "\t no detailed help available\n";
      else
	return help_str + commands[icmd]->detail + "\n";
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
