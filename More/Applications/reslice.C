/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*

Given a contiguous sequence of single-pulse sub-integrations, output
a new sequence with pulse phase x shifted to phase bin 0.

*/

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/UnloadOptions.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

using namespace std;
using namespace Pulsar;

//
//! An reslice of an application
//
class reslice : public Pulsar::Application
{
public:

  //! Default constructor
  reslice ();

  //! Process the given archive
  void process (Archive*);

  //! Return true if finished archive is ready for unloading 
  bool do_finish () { return finished; }

  virtual Archive* result () { if (!finished) return 0; return finished; }

  //! Output any finished sub-integrations in partially finished archive
  void finalize ();

protected:

  //! Shift all phase bins by the specified amount
  double shift;

  //! Ensure that sub-integrations are contiguous
  bool ensure_contiguous;

  //! Already shifted: only subint-to-subint copy required
  bool already_shifted;

  //! Add command line options
  void add_options (CommandLine::Menu&);

private:

  unsigned shift_bin;
  unsigned offset_bin;

  void copy_shift (Integration* subint);
  void copy_shift (Integration* into, Integration* from);

  Reference::To<Archive> finished;

  Reference::To<Archive> unfinished;
};


reslice::reslice ()
  : Application ("reslice", "reslice psrchive program")
{
  add( new StandardOptions );
  add( new UnloadOptions );

  shift = 0;
  ensure_contiguous = false;
  already_shifted = false;
}


void reslice::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // add a blank line and a header to the output of -h
  menu.add ("\n" "General options:");

  arg = menu.add (shift, 'p', "phase");
  arg->set_help ("shift all bins by phase (in turns)");

  arg = menu.add (ensure_contiguous, 'c', "");
  arg->set_help ("ensure that sub-integrations are contiguous");

  arg = menu.add (already_shifted, 'a', "");
  arg->set_help ("input data are already shifted in phase");
}


void reslice::copy_shift (Integration* subint)
{
  unsigned nchan = subint->get_nchan();
  unsigned npol = subint->get_npol();
  unsigned nbin = subint->get_nbin();

  for (unsigned ipol=0; ipol < npol; ipol++)
    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      Profile* profile = subint->get_Profile (ipol, ichan);
      float* amps = profile->get_amps();
      for (unsigned ibin=shift_bin; ibin<nbin; ibin++)
	amps[ibin-shift_bin] = amps[ibin];
    }
}


void reslice::copy_shift (Integration* into, Integration* from)
{
  unsigned nchan = into->get_nchan();
  unsigned npol = into->get_npol();
  unsigned nbin = into->get_nbin();

  unsigned start_bin = nbin - shift_bin;

  for (unsigned ipol=0; ipol < npol; ipol++)
    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      Profile* profile = into->get_Profile (ipol, ichan);
      float* into_amps = profile->get_amps();

      profile = from->get_Profile (ipol, ichan);
      float* from_amps = profile->get_amps();

      for (unsigned ibin=start_bin; ibin<nbin; ibin++)
	into_amps[ibin] = from_amps[ibin-start_bin+offset_bin];
    }
}


void reslice::process (Archive* archive)
{
  unsigned nbin = archive->get_nbin();
  shift_bin = nbin * shift;
  offset_bin = 0;

  if (already_shifted)
    offset_bin = nbin - shift_bin;

  if (!unfinished)
    cerr << "reslice: phase shift=" << shift 
	 << " -> " << shift_bin << " bins" << endl;

  else
  {
    unsigned nsub = unfinished->get_nsubint();

    copy_shift (unfinished->get_Integration(nsub-1), 
		archive->get_Integration(0));

    finished = unfinished;
  }

  unsigned nsub = archive->get_nsubint();

  for (unsigned isub=0; isub < nsub; isub++)
  {
    if (!already_shifted)
      copy_shift (archive->get_Integration (isub));

    if (isub+1 < nsub)
      copy_shift (archive->get_Integration (isub), 
		  archive->get_Integration(isub+1));
  }

  unfinished = archive;
}

void reslice::finalize ()
{
  if (unfinished && unfinished->get_nsubint() > 1)
  { 
    // the last sub-integration is only partially finished
    unfinished->resize( unfinished->get_nsubint() - 1 );
    finish (unfinished);
  }
}

/*!

  The standard C/C++ main function simply calls Application::main

*/

int main (int argc, char** argv)
{
  reslice program;
  return program.main (argc, argv);
}

