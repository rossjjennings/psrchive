/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "CommandLine.h"

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
void CommandLine::Menu::add (Item*)
{
}

void CommandLine::Menu::parse (int argc, char* const * argv)
{
}
