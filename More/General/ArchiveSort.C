/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ArchiveSort.h"
#include "Error.h"

#include <algorithm>
using namespace std;

Pulsar::ArchiveSort::ArchiveSort ()
{
  centre_frequency = 0.0;
}

Pulsar::ArchiveSort::ArchiveSort (istream& input)
{
  input >> filename >> source >> centre_frequency >> epoch;
  if (input.fail())
    throw Error (InvalidState, "Pulsar::ArchiveSort", "error on stream");
}

bool Pulsar::operator < (const ArchiveSort& a, const ArchiveSort& b)
{
  if (a.source < b.source)
    return true;
  if (a.centre_frequency < b.centre_frequency)
    return true;
  if (a.epoch < b.epoch)
    return true;
  return false;
}


void Pulsar::ArchiveSort::load (istream& input, vector<ArchiveSort>& entries)
try {

  string filename, name, freq, mjd;
  input >> filename >> name >> freq >> mjd;

  if (filename != "FILENAME" && filename != "filename")
    throw Error (InvalidState, "Pulsar::ArchiveSort::load",
		 "input is not the output of vap?");

  while (!input.eof())
    entries.push_back( ArchiveSort(input) );

  sort (entries.begin(), entries.end());
}
catch (Error& error) {
  cerr << "Pulsar::ArchiveSort::load " << error.get_message() << endl;
}
