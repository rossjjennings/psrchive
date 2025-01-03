/***************************************************************************
 *
 *   Copyright (C) 2019 - 2024 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Error.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Archive::tscrunched
//
Pulsar::Archive* Pulsar::Archive::tscrunched () const try
{
  vector< Reference::To<Archive> > levels (1); 

  for (unsigned isub=0; isub < get_nsubint(); isub++)
  {
    if (!levels[0])
    {
      vector<unsigned> subint (1);
      subint[0] = isub;

      if (verbose > 2)
        cerr << "Pulsar::Archive::tscrunched extracting subint=" << isub << endl;

      levels[0] = extract (subint);
      levels[0]->set_ephemeris(nullptr);
      levels[0]->set_model(nullptr);
      continue;
    }

    if (verbose > 2)
      cerr << "Pulsar::Archive::tscrunched adding clone of subint=" << isub << endl;

    levels[0]->manage( get_Integration(isub)->clone() );
    levels[0]->tscrunch ();

    unsigned level = 0;
    while (level < levels.size())
    {
      if (levels.size() == level + 1)
      {
 	if (verbose > 2)
          cerr << "Pulsar::Archive::tscrunched adding level " << level+1 << endl;
        levels.push_back(nullptr);
      }

      if (!levels[level+1])
      {
        if (verbose > 2)
          cerr << "Pulsar::Archive::tscrunched shifting up from level " << level << endl;
	levels[level+1] = levels[level];
	levels[level] = nullptr;
        break;
      }

      if (verbose > 2)
        cerr << "Pulsar::Archive::tscrunched adding subint from level " << level << endl;
      levels[level+1]->manage( levels[level]->get_Integration(0) );
      levels[level+1]->tscrunch();
      levels[level] = nullptr;
      level ++;
    }
  }

  for (unsigned level = 0; level+1 < levels.size(); level++)
  {
    if (!levels[level])
      continue;

    if (!levels[level+1])
    {
      if (verbose > 2)
        cerr << "Pulsar::Archive::tscrunched final shifting up from level " << level << endl;
      levels[level+1] = levels[level];
      levels[level] = nullptr;
      continue;
    }

    if (verbose > 2)
      cerr << "Pulsar::Archive::tscrunched adding subint from level " << level << endl;
    levels[level+1]->manage( levels[level]->get_Integration(0) );
    levels[level+1]->tscrunch();
    levels[level] = nullptr;
    level ++;
  }

  return levels.back().release();
}
catch (Error& error)
{
  throw error += "Pulsar::Archive::tscrunched";
}

