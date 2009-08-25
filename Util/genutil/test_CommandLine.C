/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "CommandLine.h"

#include <iostream>

using namespace std;

class Test
{
public:
  void parseOptions (int argc, char** argv);

  string text;
  double x;
  bool pole;
};

int main(int argc, char** argv)
{
  Test test;
  test.parseOptions (argc,argv);

  cerr << "test_CommandLine: -s " << test.text << endl;
}


void Test::parseOptions (int argc, char** argv)
{
  CommandLine::Menu menu;
  CommandLine::Argument* arg;

  menu.add (text, 's');

  arg = menu.add (x, 'x');
  arg->set_long_name ("abscissa");

  //
  // Parse the command line.
  //
  menu.parse (argc, argv);
}
