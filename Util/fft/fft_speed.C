/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "FTransformAgent.h"
#include "CommandLine.h"
#include "RealTimer.h"
#include "malloc16.h"

#include <iostream>

using namespace FTransform;
using namespace std;

class Speed : public Reference::Able
{
public:

  Speed ();

  // parse command line options
  void parseOptions (int argc, char** argv);

  // run the test
  void runTest ();

  // list the available FFT libraries
  void listLibraries();

protected:

  string library;
  unsigned nthread;
  unsigned nloop;
  unsigned nfft;
  bool real_to_complex;
};

Speed::Speed ()
{
  nloop = 1000 * 1000;
  nfft = 4;
  nthread = 1;
  real_to_complex = false;
}

int main(int argc, char** argv)
{
  Speed speed;
  speed.parseOptions (argc, argv);
  speed.runTest ();
}

void Speed::parseOptions (int argc, char** argv)
{
  CommandLine::Menu menu;
  CommandLine::Argument* arg;

  menu.set_help_header ("fft_speed - measure FFT speed");
  menu.set_version ("fft_speed version 1.0");

  arg = menu.add (this, &Speed::listLibraries, 'L');
  arg->set_help ("list available FFT libraries");

  arg = menu.add (library, 'l', "lib");
  arg->set_help ("set FFT library name");

  arg = menu.add (real_to_complex, 'r');
  arg->set_help ("real-to-complex FFT");

  arg = menu.add (nfft, 'n', "nfft");
  arg->set_help ("FFT length");

  arg = menu.add (library, 'i', "niter");
  arg->set_help ("number of iterations");

  arg = menu.add (library, 't', "nthread");
  arg->set_help ("number of threads");

  menu.parse (argc, argv);
}

void Speed::listLibraries ()
{
  unsigned nlib = FTransform::get_num_libraries ();

  for (unsigned ilib=0; ilib < nlib; ilib++)
    cout << FTransform::get_library_name (ilib) << endl;

  exit (0);
}

void Speed::runTest ()
{
  if (!library.empty())
    FTransform::set_library (library);

  FTransform::Plan* plan;
  if (real_to_complex)
    plan = Agent::current->get_plan (nfft, FTransform::frc);
  else
    plan = Agent::current->get_plan (nfft, FTransform::fcc);

  float* in = (float*) malloc16 (sizeof(float) * nfft);
  memset (in, 0, nfft*sizeof(float));
  float* out = (float*) malloc16 (sizeof(float) * (nfft+2));

  RealTimer timer;
  timer.start ();

  if (real_to_complex)
    for (unsigned i=0; i<nloop; i++)
      plan->frc1d (nfft, out, in);
  else
    for (unsigned i=0; i<nloop; i++)
      plan->fcc1d (nfft, out, in);

  timer.stop ();

  delete plan;
  free16 (in);
  free16 (out);

  cout << nfft << " " << timer.get_elapsed()/nloop << endl;
}
