/***************************************************************************
 *
 *   Copyright (C) 2002, 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "CommandParser.h"
#include "string_utils.h"

#include <fstream>

using namespace std;

bool CommandParser::debug = false;

CommandParser::CommandParser()
{
  quit = false;
  verbose = false;
}

CommandParser::~CommandParser()
{
  for (unsigned icmd=0; icmd < commands.size(); icmd++)
    delete commands[icmd];
}

void CommandParser::script (const string& filename)
{
  vector<string> lines;
  loadlines (filename, lines);

  for (unsigned i=0; i<lines.size(); i++) {
    string response = parse( lines[i] );
    if (fault())
      throw Error (InvalidState, "CommandParser::script", response);
    cout << response;
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


string CommandParser::parse (const string& cmd, const string& args)
{
  string command = cmd;
  string arguments = args;

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


  if (command == "help")
    return help (arguments);

  if (debug)
    cerr << "CommandParser::parse command not help" << endl;

  bool shortcut = command.length() == 1;

  for (unsigned icmd=0; icmd < commands.size(); icmd++)

    if ( (shortcut && command[0] == commands[icmd]->shortcut)
	 || command == commands[icmd]->command) {

      // current_command = icmd;

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

  return "invalid command: " + command + "\n";
}

/*
string CommandParser::usage ()
{
  return "usage: " + commands[current_command]->detail;
}
*/

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
