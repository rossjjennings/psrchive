/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "FTransformBench.h"

using namespace std;

FTransform::Bench::Bench ()
{
  nthread = FTransform::nthread;
  path = ".";
}

//! Set the patch from which benchmarks will be loaded
void FTransform::Bench::set_path (const std::string& _path)
{
  path = _path;
  entry.resize (0);
}

//! Set the number of threads for which benchmarks should be loaded
void FTransform::Bench::set_nthread (unsigned _nthread)
{
  nthread = _nthread;
  entry.resize (0);
}

void FTransform::Bench::load () const
{
  unsigned use_nthread = 1;
  while (use_nthread < nthread)
    use_nthread *= 2;

  unsigned nlib = FTransform::get_num_libraries ();

  for (unsigned ilib=0; ilib < nlib; ilib++)
  {
    string name = FTransform::get_library_name (ilib);
    string nt = tostring(use_nthread);

    string filename = path + "/fft_bench_" + name + "_" + nt + ".dat";

    cerr << "FTransform::Bench::load filename=" << filename << endl;
  }
}

//! Get the best FFT speed for the transform length
FTransform::Bench::Entry FTransform::Bench::get_best (unsigned nfft) const
{
  if (entry.size() == 0)
    load ();

  Entry entry;

  return entry;
}

#if 0
  protected:

    //! Database entries
    std::vector<Entry> entry;
  };

  class Bench::Entry
  {
  public:
    std::string library;
    unsigned nfft;
    double speed;
  };
#endif
