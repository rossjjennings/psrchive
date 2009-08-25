/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "CommandLine.h"

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

  return Help ( first, reminder );
}

CommandLine::Menu::~Menu ()
{
}

//! Add an item to the menu
void CommandLine::Menu::add (Item* i)
{
  item.push_back (i);
}

void CommandLine::Menu::parse (int argc, char* const * argv)
{
  struct option* longopts = 0;
  unsigned nlong = 0;

  string shortopts;

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

    longopts = (struct option*) realloc (longopts,
					 (nlong+1) * sizeof (struct option));

    longopts[nlong].name = strdup (arg->long_name.c_str());
    longopts[nlong].has_arg = arg->has_arg;
    longopts[nlong].flag = NULL;
    longopts[nlong].val = arg->val;

    nlong ++;
  }

  longopts = (struct option*) realloc (longopts,
				       (nlong+1) * sizeof (struct option));
  longopts[nlong].name = 0;
  longopts[nlong].has_arg = 0; 
  longopts[nlong].flag = NULL;
  longopts[nlong].val = 0;

  int code = 0;
  const char* optstring = shortopts.c_str();

  while ((code = getopt_long(argc, argv, optstring, longopts, NULL)) != -1) 
  {
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
