/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Agent.h"
#include "strutil.h"

using namespace std;

#define _PSRCHIVE_STATIC 1

string Pulsar::Archive::Agent::plugin_path;

bool Pulsar::Archive::Agent::loaded = Pulsar::Archive::Agent::init ();

//! Destructor
Pulsar::Archive::Agent::~Agent ()
{
  if (verbose == 3)
    cerr << "Pulsar::Archive::Agent::~Agent" << endl;
}

// reports on the status of the plugins
void Pulsar::Archive::Agent::report ()
{
  cerr << endl;

  Registry::List<Agent>& registry = Registry::List<Agent>::get_registry();

  if (verbose == 3)
    cerr << "Archive::Agent::report registry=" << &registry << endl;

  if (registry.size() == 0)
    cerr << "Archive::Agent::report No Agents registered." << endl;
  else
    cerr << "Archive::Agent::report " << registry.size() 
	 << " Registered Agents:" << endl;
  
  cout << get_list () << endl;

#ifndef _PSRCHIVE_STATIC
  plugin_report ();
#endif

}

string Pulsar::Archive::Agent::get_list ()
{
  Registry::List<Agent>& registry = Registry::List<Agent>::get_registry();

  unsigned maxlen = 0;
  unsigned agent = 0;

  for (agent=0; agent<registry.size(); agent++)
    if (registry[agent]->get_name().length() > maxlen)
      maxlen = registry[agent]->get_name().length();

  maxlen += 3;

  string out;
  for (agent=0; agent<registry.size(); agent++) {
    if (agent)
      out += "\n";
    out += 
      pad(maxlen, registry[agent]->get_name()) 
      + registry[agent]->get_description();
  }

  return out;
}

void Pulsar::Archive::Agent::get_list( vector<pair<string,string> > &details )
{
  Registry::List<Agent>& registry = Registry::List<Agent>::get_registry();

  details.resize( registry.size() );

  for( unsigned agent = 0; agent < registry.size(); agent ++ )
  {
    details[agent].first = registry[agent]->get_name();
    details[agent].second = registry[agent]->get_description();
  }
}


