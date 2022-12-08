/***************************************************************************
 *
 *   Copyright (C) 2014 by Willem van Straten and Fabian Jankowski
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DeleteInterpreter.h"

#include "Pulsar/ArchiveExpert.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/Profile.h"
#include "Pulsar/CalibratorExtension.h"

#include "TextInterface.h"
#include "pairutil.h"

#include <cassert>

using namespace std;

Pulsar::Option<bool> Pulsar::DeleteInterpreter::adjust_metadata_while_deleting_channels
(
 "DeleteInterpreter::adjust_metadata_while_deleting_channels", false,

 "Adjust bandwidth and centre frequency after deleting channels [boolean]",

 "If true, after using either delete freq or delete chan the bandwidth\n"
 "of the archive as well as its centre frequency will be updated to\n"
 "reflect the changes to the archive.\n\n"
 "Will only work on not de-dispersed archives."
);

static string index_help (const string& cmd)
{
  return
    "usage: " + cmd + " iex1 [iex2 ...]\n"
    "  string iexN   unsigned index exression: [i|i1-i2|-iend|istart-]";
}

Pulsar::DeleteInterpreter::DeleteInterpreter ()
{
  add_command
    ( &DeleteInterpreter::chan,
      "chan", "delete specified channels",
      index_help("chan") );

  add_command
    ( &DeleteInterpreter::subint,
      "subint", "delete specified integrations",
      index_help("subint") );

  add_command
    ( &DeleteInterpreter::edge,
      "edge", "delete fraction of band edges",
      "usage: edge <fraction> \n"
      "  float <fraction> fraction of band deleted from each edge \n");

  add_command
    ( &DeleteInterpreter::freq,
      "freq", "delete frequency range(s)",
      "usage: freq < MHz0:MHz1 | >MHz | <MHz > \n"
      "  float pair <MHz0:MHz1> range of frequencies to delete \n"
      "  float [>MHz] delete everything above frequency \n"
      "  float [<MHz] delete everything below frequency \n");

  add_command
    ( &DeleteInterpreter::cal,
      "cal", "delete specified channels from CalibratorExtenstion",
      index_help("cal") );

  add_command
    ( &DeleteInterpreter::bin,
      "bin", "delete specified phase bins",
      index_help("bin") );
}

Pulsar::DeleteInterpreter::~DeleteInterpreter ()
{
}

string Pulsar::DeleteInterpreter::empty ()
{
  return response (Fail, help());
}

extern void parse_indeces (vector<unsigned>& indeces,
		    const vector<string>& arguments,
		    unsigned limit);

void Pulsar::DeleteInterpreter::delete_channels (vector<unsigned>& channels)
{
  std::sort (channels.begin(), channels.end(), std::greater<unsigned>());

  Archive* data = get();

  // delete selected channels in all sub-integrations
  unsigned nsubint = data->get_nsubint();

  if (nsubint == 0)
    return;

  unsigned new_nchan = 0;

  for (unsigned isub=0; isub<nsubint; isub++)
  {
    Integration* subint = data->get_Integration(isub);
    for (unsigned i=0; i<channels.size(); i++)
      subint->expert()->remove( channels[i] );

    if (isub == 0)
    {
      new_nchan = subint->get_nchan();
    }
    else
    {
      assert (new_nchan == subint->get_nchan());
    }
  }

  data->expert()->set_nchan( new_nchan );
}

string Pulsar::DeleteInterpreter::chan (const string& args) try 
{
  double org_bw = get()->get_bandwidth();
  unsigned org_nchan = get()->get_nchan();
  double chan_bw = org_bw / (double)org_nchan;
  vector<string> arguments = setup (args);

  vector<unsigned> channels;
  parse_indeces (channels, arguments, org_nchan);

  delete_channels (channels);

  if (adjust_metadata_while_deleting_channels)
  {
    get()->set_bandwidth(org_bw - (double)channels.size() * chan_bw);
    // Do we always want to reset center freq?
    if (get()->get_nchan()>0) {
      get()->update_centre_frequency();
    }
  }

  return response (Good);
}
catch (Error& error) {
  return response (error);
}


string Pulsar::DeleteInterpreter::subint (const string& args) try 
{
  vector<string> arguments = setup (args);

  vector<unsigned> subints;
  parse_indeces (subints, arguments, get()->get_nsubint());

  std::sort (subints.begin(), subints.end(), std::greater<unsigned>());

  // delete all selected sub-integrations
  for (unsigned i=0; i<subints.size(); i++)
    get()->erase (subints[i]);

  return response (Good);
}
catch (Error& error) {
  return response (error);
}

// //////////////////////////////////////////////////////////////////////
//
string Pulsar::DeleteInterpreter::edge (const string& args)
try {

  float fraction = setup<float> (args);

  if (fraction <= 0.0 || fraction >= 0.5)
    return response (Fail, "invalid fraction " + tostring(fraction));

  unsigned nchan = get()->get_nchan();
  unsigned nedge = unsigned( nchan * fraction );

  vector<unsigned> channels;

  for (unsigned ichan=0; ichan < nedge; ichan++)
    channels.push_back (ichan);

  for (unsigned ichan=nchan-nedge; ichan < nchan; ichan++)
    channels.push_back (ichan);

  delete_channels (channels);
  return response (Good);

}
catch (Error& error)
{
  return response (error);
}

#include "Ranges.h"

// //////////////////////////////////////////////////////////////////////
//
string Pulsar::DeleteInterpreter::freq (const string& args) try
{
  double org_bw = get()->get_bandwidth();
  unsigned org_nchan = get()->get_nchan();
  double chan_bw = org_bw / (double)org_nchan;
  unsigned removed_channels_count = 0;
  vector<string> arguments = setup (args);

  for (unsigned iarg=0; iarg < arguments.size(); iarg++)
  {
    Range r = fromstring<Range> (arguments[iarg]);

    Archive* archive = get();

    unsigned nsub = archive->get_nsubint();

    for (unsigned isub=0; isub < nsub; isub++)
    {
      Integration* subint = archive->get_Integration (isub);
      for (unsigned ichan=0; ichan < subint->get_nchan(); )
        if (r.within( subint->get_centre_frequency(ichan) )) {
          subint->expert()->remove (ichan);
          if (isub == 0)
            // only count the removed channels once
            removed_channels_count++;
        }
        else
          ichan ++;
    }
  }

  unsigned new_nchan = get()->get_Integration(0)->get_nchan();
  if (get()->get_nsubint() > 0)
    get()->expert()->set_nchan( new_nchan );

  if (adjust_metadata_while_deleting_channels)
  {
    get()->set_bandwidth(org_bw - (double)removed_channels_count * chan_bw);
    // Do we always want to reset center freq?
    if (new_nchan>0) {
      get()->update_centre_frequency();
    }
  }

  return response (Good);
}
catch (Error& error)
{
  return response (error);
}


string Pulsar::DeleteInterpreter::cal (const string& args) try 
{
  // cerr << "Pulsar::DeleteInterpreter::cal args='" << args << "'" << endl;

  Reference::To<CalibratorExtension> ext = get()->get<CalibratorExtension>();
  if (!ext)
    return response (Fail, "archive does not contain CalibratorExtension");

  vector<string> arguments = setup (args);

  vector<unsigned> channels;
  parse_indeces (channels, arguments, ext->get_nchan());

  std::sort (channels.begin(), channels.end(), std::greater<unsigned>());

  // delete selected channels in CalibratorExtension
  for (unsigned i=0; i<channels.size(); i++)
  {
    // cerr << "delete cal " << channels[i] << endl;
    ext->remove_chan( channels[i], channels[i] );
  }
 
  return response (Good);
}
catch (Error& error)
{
  return response (error);
}


string Pulsar::DeleteInterpreter::bin (const string& args) try 
{
  vector<string> arguments = setup (args);

  Archive* archive = get();
  unsigned nsub = archive->get_nsubint();
  unsigned nchan = archive->get_nchan();
  unsigned npol = archive->get_npol();
  unsigned nbin = archive->get_nbin();

  vector<unsigned> bins;
  parse_indeces (bins, arguments, archive->get_nbin());

  std::sort (bins.begin(), bins.end());

  // find the number of contiguous bins deleted from the start of the profile
  unsigned ioffset = 0;
  while (ioffset < bins.size() && bins[ioffset] == ioffset) ioffset++;

  if (ioffset < bins.size())
  {
    // ensure that the remaining bins deleted are contiguous to the end
    unsigned iend = ioffset;
    while (iend+1 < bins.size() && bins[iend]+1 == bins[iend+1]) iend++;

    if (bins[iend] != nbin-1)
      return response (Fail, "bins do not contiguously span phase zero");
  }

  unsigned new_nbin = nbin - bins.size();

  for (unsigned isub=0; isub < nsub; isub++)
  {
    Integration* subint = archive->get_Integration (isub);
    double period = subint->get_folding_period ();

    if (ioffset)
    {
      double offset = (period * ioffset) / nbin;
      cerr << "adjusting start time by " << offset << " seconds" << endl;
      subint->set_epoch( subint->get_epoch() + offset );
    }

    double new_period = (period * new_nbin) / nbin;
    cerr << "setting new folding period = " << new_period << endl;
    subint->set_folding_period (new_period);

    for (unsigned ipol=0; ipol < npol; ipol++)
      for (unsigned ichan=0; ichan < nchan; ichan++)
      {
	Profile* profile = subint->get_Profile (ipol, ichan);
	profile->remove (bins);
	assert (profile->get_nbin() == new_nbin);
      }

    subint->expert()->set_nbin(new_nbin);
  }

  archive->expert()->set_nbin(new_nbin);

  return response (Good);
}
catch (Error& error) {
  return response (error);
}
