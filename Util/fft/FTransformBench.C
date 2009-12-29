/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "FTransformBench.h"
#include "debug.h"

#include <fstream>
#include <math.h>

using namespace std;

FTransform::Bench::Bench ()
{
  nthread = FTransform::nthread;
  path = ".";
  loaded = false;
  max_nfft = 0;
}

//! Set the patch from which benchmarks will be loaded
void FTransform::Bench::set_path (const std::string& _path)
{
  path = _path;
  reset ();
}

//! Set the number of threads for which benchmarks should be loaded
void FTransform::Bench::set_nthread (unsigned _nthread)
{
  nthread = _nthread;
  reset ();
}

void FTransform::Bench::reset ()
{
  entries.resize (0);
  max_nfft = 0;
  loaded = false;
}

void FTransform::Bench::load () const
{
  unsigned use_nthread = 1;
  while (use_nthread < nthread)
    use_nthread *= 2;

  max_nfft = 0;

  unsigned nlib = FTransform::get_num_libraries ();

  for (unsigned ilib=0; ilib < nlib; ilib++) try
  {
    string library = FTransform::get_library_name (ilib);
    string nt = tostring(use_nthread);

    string filename = path + "/fft_bench_" + library + "_" + nt + ".dat";

    DEBUG("FTransform::Bench::load filename=" << filename);

    load (library, filename);
  }
  catch (Error& error)
  {
    cerr << error << endl;
  }

  loaded = true;
}

void FTransform::Bench::load (const std::string& library,
			      const std::string& filename) const
{
  ifstream in (filename.c_str());
  if (!in)
    throw Error (FailedSys, "FTransform::Bench::load", "std::ifstream");

  while (!in.eof())
  {
    Entry entry;
    double log2nfft, mflops;

    in >> entry.nfft >> entry.speed >> log2nfft >> mflops;

    if (in.eof())
      continue;

    entry.library = library;
    
    DEBUG(library << " " << entry.nfft << " " << entry.speed);

    entries.push_back (entry);

    if (entry.nfft > max_nfft)
      max_nfft = entry.nfft;
  }
}

double theoretical (unsigned nfft)
{
  return 5.0 * nfft * log2 (nfft);
}

//! Get the best FFT speed for the transform length
FTransform::Bench::Entry FTransform::Bench::get_best (unsigned nfft) const
{
  if (!loaded)
    load ();

  unsigned use_nfft = nfft;

  if (nfft > max_nfft)
    use_nfft = max_nfft;

  Entry entry;

  for (unsigned i=0; i<entries.size(); i++)
    if ( entries[i].nfft == use_nfft &&
	 (entry.nfft == 0 || entries[i].speed < entry.speed) )
      entry = entries[i];

  if (entry.nfft == 0)
  {
    entry.library = "theoretical";
    entry.nfft = nfft;
    entry.speed = theoretical (nfft);
  }

  return entry;
}

