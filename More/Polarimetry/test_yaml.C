/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <yaml-cpp/yaml.h>
#include <iostream>

using namespace std;

void print (const YAML::Node& node, string indent)
{
  if (node.IsSequence())
  {
    cout << indent << "sequence of " << node.size() << " nodes" << endl;
    for (std::size_t i=0; i<node.size(); i++)
      print (node[i], indent + " ");
  }

  else if (node.IsMap())
  {
    cout << indent << "map of " << node.size() << " pairs" << endl;
    for (auto it=node.begin(); it!=node.end(); ++it)
    {
      cout << indent+" " << "1:" << endl;
      print (it->first, indent+" ");
      cout << indent+" " << "2:" << endl;
      print (it->second, indent+" ");
    }
  }
  else if (node.IsScalar())
    cout << indent << node.as<std::string>() << endl;
}

int main (int argc, char** argv)
{
  if (argc < 2)
  {
    cerr << "please provide a yaml filename" << endl;
    return -1;
  }

  string filename = argv[1];

  YAML::Node node = YAML::LoadFile(filename);
  print (node, "");
  return 0;
}

