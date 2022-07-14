/***************************************************************************
 *
 *   Copyright (C) 2006 - 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ArchiveSort.h"
#include "Error.h"

using namespace std;

bool Pulsar::ArchiveSort::read_length = false;
bool Pulsar::ArchiveSort::compare_frequencies = true;
bool Pulsar::ArchiveSort::compare_sources = true;

Pulsar::ArchiveSort::ArchiveSort ()
{
  centre_frequency = 0.0;
}

Pulsar::ArchiveSort::ArchiveSort (istream& input)
{
  input >> filename >> source >> centre_frequency >> epoch;
  if (read_length)
    input >> length;
  if (input.fail())
    throw Error (InvalidState, "Pulsar::ArchiveSort", "error on stream");
}

bool Pulsar::ArchiveSort::verbose = false;

bool Pulsar::operator < (const ArchiveSort& a, const ArchiveSort& b)
{
  if (ArchiveSort::compare_sources)
  {
    if (a.source < b.source)
    {
      if (ArchiveSort::verbose)
        cerr << a.source << " < " << b.source << endl;
      return true;
    }
    else if (a.source > b.source)
    {
      if (ArchiveSort::verbose)
        cerr << a.source << " > " << b.source << endl;
      return false;
    }
  }

  if (ArchiveSort::compare_frequencies)
  {
    if (a.centre_frequency < b.centre_frequency)
    {
      if (ArchiveSort::verbose)
	cerr << a.centre_frequency << " < " << b.centre_frequency << endl;
      return true;
    }
    else if (a.centre_frequency > b.centre_frequency)
    {
      if (ArchiveSort::verbose)
	cerr << a.centre_frequency << " > " << b.centre_frequency << endl;
      return false;
    }
  }
  
  if (a.epoch < b.epoch)
  {
    if (ArchiveSort::verbose)
      cerr << a.epoch.printdays(5) << " < " << b.epoch.printdays(5) << endl;
    return true;
  }

  if (ArchiveSort::verbose)
    cerr << "not less than" << endl;
  return false;
}


void Pulsar::ArchiveSort::load (istream& input, list<ArchiveSort>& entries)
try
{
  string filename, name, freq, mjd, length;
  input >> filename >> name >> freq >> mjd;
  if (read_length)
    input >> length;

  if (filename != "FILE" && filename != "filename")
    throw Error (InvalidState, "Pulsar::ArchiveSort::load",
		 "input is not the output of vap?");

  while (!input.eof())
  {
    ArchiveSort entry (input);
    entries.push_back( entry );
  }
}
catch (Error& error)
{
  cerr << "Pulsar::ArchiveSort::load " << error.get_message() << endl;
}

