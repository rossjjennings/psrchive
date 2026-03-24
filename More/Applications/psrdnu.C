/***************************************************************************
 *
 *   Copyright (C) 2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/UnloadOptions.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/ProfileStats.h"
#include "Pulsar/Statistics.h"
#include "Pulsar/FortranSNR.h"

using namespace std;
using namespace Pulsar;

//
//! Searches for the frequency offset that maximizes S/N
//
class psrdnu : public Pulsar::Application
{
public:

  //! Default constructor
  psrdnu ();

  //! Process the given archive
  void process (Pulsar::Archive*) override;

  //! Add command line options
  void add_options (CommandLine::Menu&) override;

protected:

  //! number of frequency trials
  int ntrial = 100;

  //! expression used to evaluate each trial
  std::string expression;
};


psrdnu::psrdnu ()
  : Application ("psrdnu", "search for frequency offset that maximizes S/N")
{
  add( new Pulsar::StandardOptions );
  add( new Pulsar::UnloadOptions );
}

void psrdnu::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // add a blank line and a header to the output of -h
  menu.add ("\n" "General options:");

  arg = menu.add (ntrial, 'n', "trials");
  arg->set_help ("number of trials");

  arg = menu.add (expression, "exp", "string");
  arg->set_help ("expression to evaluate for each trial");
}

void psrdnu::process (Pulsar::Archive* archive)
{
  Reference::To<Archive> clone = archive->clone();
  clone->tscrunch();
  clone->pscrunch();

  double bw = archive->get_bandwidth();
  double freq = archive->get_centre_frequency();
  unsigned nchan = archive->get_nchan();
  double chan_bw = bw / nchan;

  double dnu = chan_bw / ntrial;

  Reference::To<ProfileStats> stats = new ProfileStats;
  auto parser = stats->get_interface ();
  parser->set_prefix_name(false);

  // same on used by pdmp
  FortranSNR snr_obj;

  for (int i=-ntrial/2; i<ntrial/2; i++)
  {
    double new_freq = freq + i*dnu;
    double freq0 = new_freq - 0.5*(bw-chan_bw);

    Reference::To<Archive> trial = clone->clone();
    auto subint = trial->get_Integration(0);
    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      subint->set_centre_frequency(ichan, freq0 + ichan*chan_bw);
    }

    trial->set_centre_frequency(new_freq);
    trial->dedisperse();
    trial->fscrunch();

    subint = trial->get_Integration(0);
    auto profile = subint->get_Profile(0,0);

    double stat = 0.0;

    if (expression.empty())
    {
      snr_obj.set_rms(1.0);
      snr_obj.set_minwidthbins (1);
      // snr_obj.set_maxwidthbins (p->get_nbin()/2);
      stat = snr_obj.get_snr (profile);
    }
    else
    {
      stats->set_Integration(subint);
      stats->set_Profile (profile);
      string value = ::process( parser, expression );
      stat = fromstring<double>( value );
    }

    cout << tostring(new_freq,10) << " " << stat << endl;
  }
}

int main (int argc, char** argv)
{
  psrdnu program;
  return program.main (argc, argv);
}

