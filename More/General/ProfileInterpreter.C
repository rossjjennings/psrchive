/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileInterpreter.h"
#include "Pulsar/ArchiveTemplates.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include "Pulsar/RemoveBaseline.h"
#include "Pulsar/PolnProfileStats.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/Index.h"

#include "Pulsar/Accumulate.h"
#include "Pulsar/Differentiate.h"
#include "Pulsar/Detrend.h"
#include "Pulsar/SmoothMean.h"
#include "Pulsar/SmoothMedian.h"
#include "Pulsar/SmoothSinc.h"

#include "Pulsar/Extract.h"
#include "Pulsar/Subtract.h"
#include "Pulsar/Convolve.h"
#include "Pulsar/Correlate.h"

#include "pairutil.h"

using namespace std;

string help_smooth (const string& method)
{
  return
    "usage: " + method + " <factor> \n"
    "where: <factor> is the width of the smoothing function \n"
    "       if factor < 1, then it is interpreted as turns \n"
    "       if factor > 1, then it is interpreted as phase bins \n";
}

Pulsar::ProfileInterpreter::ProfileInterpreter ()
{
  remove_baseline = new RemoveBaseline::Total;

  add_command 
    ( &ProfileInterpreter::baseline,
      "baseline", "remove the profile baseline",
      "usage: baseline \n" );

  add_command 
    ( &ProfileInterpreter::scale,
      "scale", "scale by a mathematical expression",
      "usage: scale <exp>\n" );

  add_command 
    ( &ProfileInterpreter::mean,
      "mean", "form the mean smoothed profile",
      help_smooth ("mean") );

  add_command 
    ( &ProfileInterpreter::median,
      "median", "form the median smoothed profile",
      help_smooth ("median") );

  add_command 
    ( &ProfileInterpreter::sinc,
      "sinc", "form the low-pass filtered profile",
      help_smooth ("sinc") );

  add_command 
    ( &ProfileInterpreter::cumulative,
      "cumulative", "form the cumulative profile",
      "usage: cumulative\n" );

  add_command 
    ( &ProfileInterpreter::difference,
      "difference", "form the difference profile",
      "usage: difference [phase] \n" );

  add_command
    ( &ProfileInterpreter::detrend,
      "detrend", "detrend the profile",
      "usage: detrend\n" );

  add_command
    ( &ProfileInterpreter::extract,
      "extract", "extract (and keep) the specified phase bin range",
      "usage: extract <first:last>\n" );

  add_command
    ( &ProfileInterpreter::subtract,
      "subtract", "subtract the named archive from the current",
      "usage: subtract <name>\n" );

  add_command
    ( &ProfileInterpreter::convolve,
      "convolve", "convolve the named archive with the current",
      "usage: convolve <name>\n" );

  add_command
    ( &ProfileInterpreter::correlate,
      "correlate", "correlate the named archive with the current",
      "usage: correlate <name>\n" );

}

Pulsar::ProfileInterpreter::~ProfileInterpreter ()
{
}

string Pulsar::ProfileInterpreter::baseline (const string& args) try
{
  if (args.empty())
    remove_baseline->transform( get() );
  else
    remove_baseline->set_operation( RemoveBaseline::Operation::factory(args) );

  return response (Good);
}
catch (Error& error) {
  return response (error);
}

// defined in More/General/standard_interface.C
std::string process (TextInterface::Parser* interface, const std::string& txt);

string Pulsar::ProfileInterpreter::scale (const string& args) try
{
  if (args.empty())
    return response (Fail, "no expression specified");

  string expression = args;
  Archive* archive = get();

  const unsigned npol = archive->get_npol();
  const unsigned nchan = archive->get_nchan();
  const unsigned nsubint = archive->get_nsubint();

  Reference::To<ProfileStats> stats = new ProfileStats;
  Reference::To<TextInterface::Parser> parser = stats->get_interface ();

  Reference::To<PolnProfileStats> pstats;

  if (archive->get_state() == Signal::Stokes)
  {
    pstats = new PolnProfileStats;
    parser->insert( pstats->get_interface() );
  }

  parser->set_prefix_name(false);

  for (unsigned isubint=0; isubint < nsubint; isubint++)
  {
    Integration* subint = archive->get_Integration (isubint);

    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      if (subint->get_weight (ichan) == 0.0)
        continue;

      Index ipol (0, true); // return the integrated intensity
      Reference::To<const Profile> prof = get_Profile (subint, ipol, ichan);
      stats->set_profile (prof);

      Reference::To<const PolnProfile> pprof;
      if (pstats)
      {
        pprof = subint->new_PolnProfile (ichan);
        pstats->set_profile (pprof);
      }

      string text = process( parser, expression );
      float value = fromstring<float>( text );

      // cerr << isubint << " " << ichan << " " << text << " " << value << endl;

      for (unsigned ipol=0; ipol < npol; ipol++)
        subint->get_Profile (ipol, ichan)->scale (value);
    }
  }

  return response (Good);
}
catch (Error& error)
{
  return response (error);
}

string Pulsar::ProfileInterpreter::cumulative (const string& args) try
{
  foreach (get(), new Accumulate);
  return response (Good);
}
catch (Error& error) {
  return response (error);
}

string Pulsar::ProfileInterpreter::difference (const string& args) try
{
  unsigned span = 1;

  if (args.length())
  {
    Phase::Value phase = setup<Phase::Value> (args);
    phase.set_nbin( get()->get_nbin() );
    span = phase.get_bin();
  }

  foreach (get(), new Differentiate(span) );
  return response (Good);
}
catch (Error& error) {
  return response (error);
}

string Pulsar::ProfileInterpreter::detrend (const string& args) try
{
  foreach (get(), new Detrend);
  return response (Good);
}
catch (Error& error) {
  return response (error);
}

string Pulsar::ProfileInterpreter::mean (const string& args)
{ 
  return smooth (new SmoothMean, args);
}

string Pulsar::ProfileInterpreter::median (const string& args)
{ 
  return smooth (new SmoothMedian, args);
}

string Pulsar::ProfileInterpreter::sinc (const string& args)
{ 
  return smooth (new SmoothSinc, args);
}

string Pulsar::ProfileInterpreter::smooth (Smooth* smooth, const string& args)
try {

  Reference::To<Smooth> smooth_ref = smooth;

  float factor = setup<float> (args);

  if (factor < 1.0)
    smooth->set_turns (factor);
  else
    smooth->set_bins (factor);

  foreach( get(), smooth );
  return response (Good);

}
catch (Error& error) {
  return response (error);
}

string Pulsar::ProfileInterpreter::extract (const string& args) try
{
  typedef std::pair<unsigned,unsigned> range;
  range bins = setup<range> (args);

  Archive* data = get();
  foreach( data, new Extract(bins) );

  data->resize( data->get_nsubint(), data->get_npol(), data->get_nchan(),
                bins.second - bins.first );

  return response (Good);
}
catch (Error& error) {
  return response (error);
}

string Pulsar::ProfileInterpreter::subtract (const string& args) try
{ 
  string name = setup<string> (args);
  foreach (get(), getmap(name), new Subtract);
  return response (Good);
}
catch (Error& error) {
  return response (error);
}

string Pulsar::ProfileInterpreter::convolve (const string& args) try
{
  if (args.empty())
  {
    // if no archive name is specified, then auto-convolve
    foreach (get(), get(), new Convolve);
  }
  else
  {
    string name = setup<string> (args);
    foreach (get(), getmap(name), new Convolve);
  }

  return response (Good);
}
catch (Error& error) {
  return response (error);
}

string Pulsar::ProfileInterpreter::correlate (const string& args) try
{
  if (args.empty())
  {
    // if no archive name is specified, then auto-correlate
    foreach (get(), get(), new Correlate);
  }
  else
  {
    string name = setup<string> (args);
    foreach (get(), getmap(name), new Correlate);
  }

  return response (Good);
}
catch (Error& error) {
  return response (error);
}

string Pulsar::ProfileInterpreter::empty ()
{
  return response (Fail, help());
}


