/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
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
  if (verbose == 3)
    cerr << "Pulsar::Archive::tscrunched extracting first sub-integration" 
         << endl;

  vector<unsigned> first (1);
  first[0] = 0;
  Reference::To<Archive> copy = extract (first);

  if (verbose == 3)
    cerr << "Pulsar::Archive::tscrunch adding " << get_nsubint()-1
         << " Integration clones" << endl;

  for (unsigned isub=1; isub < get_nsubint(); isub++)
  {
    copy->manage( get_Integration(isub)->clone() );
    copy->tscrunch ();
  }

  return copy.release();
}
catch (Error& error)
{
  throw error += "Pulsar::Archive::tscrunch";
}

