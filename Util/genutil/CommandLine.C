/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "CommandLine.h"
#include "pad.h"

#include <stdlib.h>

using namespace std;

//! Return two columns of help text
CommandLine::Help CommandLine::Argument::get_help () const 
{
  string first;
  if ( !short_name.empty() )
    first += "-" + short_name;

  if ( !long_name.empty() )
  {
    if (!first.empty())
      first += ",";

    first += "--" + long_name;
  }

  if ( !type.empty() )
    first += " " + type;

  return Help ( first, help );
}

CommandLine::Menu::~Menu ()
{
}

//! Add an item to the menu
void CommandLine::Menu::add (Item* i)
{
  item.push_back (i);
}

template<typename T>
void resize (T* &ptr, unsigned size)
{
  ptr = (T*) realloc (ptr, size * sizeof(T));
}

void CommandLine::Menu::parse (int argc, char* const * argv)
{
  struct option* longopts = 0;
  unsigned nlong = 0;

  string shortopts;

  int help_val = 0;
  int version_val = 0;

  if (!help_header.empty())
  {
    shortopts += "h";

    resize (longopts, nlong+1);

    longopts[nlong].name = strdup ("help");
    longopts[nlong].has_arg = optional_argument;
    longopts[nlong].flag = NULL;
    longopts[nlong].val = help_val = 'h';

    nlong ++;
  }

  if (!version_info.empty())
  {
    shortopts += "i";

    resize (longopts, nlong+1);

    longopts[nlong].name = strdup ("version");
    longopts[nlong].has_arg = no_argument;
    longopts[nlong].flag = NULL;
    longopts[nlong].val = version_val = 'i';

    nlong ++;
  }

  for (unsigned i=0; i<item.size(); i++)
  {
    Argument* arg = dynamic_cast<Argument*>( item[i] );
    if (!arg)
      continue;

    if (arg->short_name.empty())
      arg->val = nlong + 1024;
    else
    {
      arg->val = arg->short_name[0];
      shortopts += arg->short_name;
      if (arg->has_arg == required_argument)
	shortopts += ":";
    }

    if (arg->long_name.empty())
      continue;

    resize (longopts, nlong+1);
    
    longopts[nlong].name = strdup (arg->long_name.c_str());
    longopts[nlong].has_arg = arg->has_arg;
    longopts[nlong].flag = NULL;
    longopts[nlong].val = arg->val;

    nlong ++;
  }

  resize (longopts, nlong+1);
    
  longopts[nlong].name = 0;
  longopts[nlong].has_arg = 0; 
  longopts[nlong].flag = NULL;
  longopts[nlong].val = 0;

  int code = 0;
  const char* optstring = shortopts.c_str();

  while ((code = getopt_long(argc, argv, optstring, longopts, NULL)) != -1) 
  {
    if (code == help_val)
      help (optarg);

    if (code == version_val)
      version ();

    for (unsigned i=0; i<item.size(); i++)
    {
      if (item[i]->matches (code))
      {
	string arg;
	if (optarg)
	  arg = optarg;

	item[i]->handle (arg);
	break;
      }
    }
  }
}

void CommandLine::Menu::help (const char* name)
{
  if (name)
  {
    // cerr << "CommandLine::Menu::help name='" << name << "'" << endl;

    for (unsigned i=0; i<item.size(); i++)
    {
      Argument* arg = dynamic_cast<Argument*>( item[i] );
      if (!arg)
	continue;

      std::string prefix;

      if (arg->short_name == name)
	prefix = "-";

      if (arg->long_name == name)
	prefix = "--";

      if (!prefix.empty())
      {
	if (arg->long_help.empty())
	  cout << "no further help available for " << prefix << name << endl;
	else
	  cout << prefix << name << " help:\n\n" << arg->long_help << endl;

	exit (0);
      }
    }
  }


  size_t max_length = 0;

  for (unsigned i=0; i<item.size(); i++)
  {
    Help help = item[i]->get_help();
    if (!help.second.empty())
      max_length = std::max (max_length, help.first.length());
  }

  cout << help_header << endl << endl;

  for (unsigned i=0; i<item.size(); i++)
  {
    Help help = item[i]->get_help();
    cout << pad(max_length, help.first) << "   " << help.second << endl;
  }

  cout << help_footer << endl;

  exit (0);
}

void CommandLine::Menu::version ()
{
  cout << version_info << endl;
  exit (0);
}


