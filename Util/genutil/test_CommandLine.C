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

  Test () { x = 0.0; flag = false; }

  void parseOptions (int argc, char** argv);

  string text;
  double x;
  vector<int> indeces;
  bool flag;
};

int main(int argc, char** argv)
{
  Test test;
  test.parseOptions (argc,argv);

  cerr << "test_CommandLine:\n"
    " -s " << test.text << "\n"
    " -x " << test.x << "\n"
    " -i " << test.indeces.size() << "\n"
    " -f " << test.flag << endl;
}


void Test::parseOptions (int argc, char** argv)
{
  CommandLine::Menu menu;
  CommandLine::Argument* arg;

  menu.add (flag, 'f');
  menu.add (text, 's');
  menu.add (x, 'x');

  arg = menu.add (indeces, 'i');
  arg->set_long_name ("index");

  //
  // Parse the command line.
  //
  menu.parse (argc, argv);
}