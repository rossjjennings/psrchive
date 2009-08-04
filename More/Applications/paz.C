/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// A simple command line tool for zapping RFI

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/UnloadOptions.h"

#include "Pulsar/RFIMitigation.h"
#include "Pulsar/ChannelZapModulation.h"
#include "Pulsar/ChannelZapMedian.h"
#include "Pulsar/LawnMower.h"

#include "Pulsar/StandardSNR.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/ProcHistory.h"

#include "Error.h"
#include "dirutil.h"
#include "strutil.h"

#include <unistd.h>
#include <string.h>

#include "BoxMuller.h"

//! Pulsar Archive Zapping application
class paz: public Pulsar::Application
{
public:

  //! Default constructor
  paz ();

  //! Return usage information 
  std::string get_usage ();

  //! Return getopt options
  std::string get_options ();

  //! Parse a command line option
  bool parse (char code, const std::string& arg);

  //! Verify setup
  void setup ();

  //! Process the given archive
  void process (Pulsar::Archive*);
};

int main (int argc, char** argv)
{
  paz program;
  return program.main (argc, argv);
}

char backward_compatibility (char c)
{
  // paz used to accept -u to specify the output directory, now it is -O
  if (c == 'u')
    return 'O';

  return c;
}

paz::paz () : Pulsar::Application ("paz", "zaps RFI in archives")
{
  has_manual = true;
  version = "$Id: paz.C,v 1.57 2009/08/04 06:15:12 straten Exp $";
  filter = backward_compatibility;

  add( new Pulsar::StandardOptions );
  add( new Pulsar::UnloadOptions );
}

using namespace std;

std::string paz::get_options ()
{
  return "8bC:B:dDE:f:F:Iik:l:Lno:p:P:rR:s:S:u:w:W:x:X:z:Z:";
}

std::string paz::get_usage ()
{
  return
    "Manual zapping options:\n"
    "  -I               Zero weight the intersection of -[wW] and -[zZk]\n"
    "  -k filename      Zero weight chans listed in this kill file\n"
    "  -z \"a b c ...\"   Zero weight these particular channels\n"
    "  -Z \"a b\"         Zero weight chans between a & b inclusive\n"
    "  -f \"a b c ...\"   Zero weight channels with these frequencies (MHz)\n"
    "  -F \"a b\"         Zero weight this frequency range, inclusive (MHz)\n"
    "  -x \"a b c ...\"   Delete all sub-integrations except these\n"
    "  -X \"a b\"         Delete all sub-ints except a to b inclusive\n"
    "  -E percent       Zero weight this much of each band edge\n"
    "  -s \"a b c ...\"   Delete these sub-integrations\n"
    "  -S \"a b\"         Delete sub-ints between a & b inclusive\n"
    "  -w \"a b c ...\"   Zero weight these sub-integrations\n"
    "  -W \"a b\"         Zero weight sub-ints between a & b inclusive\n"
    "  -p \"p i\"         Interpolate over every p-th phase bin, start at i\n"
    "\n"
    "Automatic zapping algorithms:\n"
    "  -r               Zap channels using median smoothed difference\n"
    "  -R size          Set the size of the median smoothing window\n"
    "  -l subint        Mow the lawn of the specified subint \n"
    "  -L               Mow the lawn of all subints \n"
    "  -d               Zero weight chans using mean offset rejection\n"
    "  -C cutoff        Zero weight chans based on S/N (std optional)\n"
    "  -P stdfile       Use this standard profile\n"
    "  -o cutoff sigma  Zero weight subints with negative dropouts\n"
    "  -8               Fix ATNF WBCORR 8 bin problem (see also -p)\n"
    "\n"
    "The format of the kill file used with the -k option is simply\n"
    "a list of channel numbers, separated by white space\n"
    "\n"
    "The cutoff S/N value used with -C is largely arbitrary. You will\n"
    "need to experiment to find the best value for your archives\n";
}

void zap_periodic_spikes (Pulsar::Profile * profile, int period, int phase, 
    int width=1);
void binzap (Pulsar::Archive * arch, Pulsar::Integration * integ, int subint,
	     int lower_bin, int upper_bin, int lower_range, int upper_range);

struct FreqRange {
  double lo;
  double hi;
};


bool bin_zap = false;
vector < int >bins_to_zap;

bool verbose = false;
bool eightBinZap = false;
vector < string > archives;

string ulpath;

string killfile;

char *metafile = NULL;

bool zero_channels = false;
vector < int >chans_to_zap;
vector < double >freqs_to_zap;
vector < FreqRange >freq_ranges_to_zap;

bool zero_subints = false;
vector < unsigned >subs_to_zap;

bool zero_intersection = false;

bool zap_subints = false;
bool nozap_subints = false;
vector < unsigned >subs_nozap;

bool edge_zap = false;
float percent = 0.0;

bool simple = false;

bool zap_ston = false;
double ston_cutoff = 0.0;

bool std_given = false;
Reference::To < Pulsar::Profile > thestd;

bool periodic_zap = false;
int periodic_zap_period = 8, periodic_zap_phase = 0, periodic_zap_width = 1;

bool dropout_zap = false;
float dropout_sigma = 5.0;

string ext;

int placeholder;
double placeholder_f;

int gotc = 0;
char *key = NULL;
char whitespace[5] = " \n\t";

Pulsar::LawnMower * mower = 0;
vector < unsigned >subints_to_mow;

Pulsar::StandardSNR standard_snr;
Pulsar::ChannelZapMedian * median_zapper = 0;
Pulsar::ChannelZapModulation * modulation_zapper = 0;

string command = "paz";

//! Parse a command line option
bool paz::parse (char code, const std::string& arg)
{
  char* c_arg = const_cast<char*>( arg.c_str() );

  switch (code)
    {
    case 'I':
      zero_intersection = true;
      command += " -I";
      break;

    case 'k':
      killfile = c_arg;
      zero_channels = true;
      command += " -k ";
      command += c_arg;
      break;

    case 'l':
      command += " l ";
      command += c_arg;
      subints_to_mow.push_back (atoi (c_arg));
      mower = new Pulsar::LawnMower;
      break;
    case 'L':
      command += " -L ";
      mower = new Pulsar::LawnMower;
      break;

    case 'z':
      command += " -z ";
      command += c_arg;
      key = strtok (c_arg, whitespace);
      zero_channels = true;
      while (key) {
	if (sscanf (key, "%d", &placeholder) == 1) {
	  chans_to_zap.push_back (placeholder);
	}
	key = strtok (NULL, whitespace);
      }
      break;

    case 'f':
      command += " -f ";
      command += c_arg;
      key = strtok (c_arg, whitespace);
      zero_channels = true;
      while (key) {
	if (sscanf (key, "%lf", &placeholder_f) == 1) {
	  freqs_to_zap.push_back (placeholder_f);
	}
	key = strtok (NULL, whitespace);
      }
      break;

    case 'B':
      command += " -B ";
      command += c_arg;
      key = strtok (c_arg, whitespace);
      bin_zap = true;
      while (key) {
	if (sscanf (key, "%d", &placeholder) == 1) {
	  bins_to_zap.push_back (placeholder);
	}
	key = strtok (NULL, whitespace);
      }
      break;

    case '8':
      // The aim here is to zero every eight time bins starting from number 6
      eightBinZap = true;
      command += " -8";
      break;

    case 'p':
      {
	periodic_zap = true;

        int rv = sscanf (c_arg, "%d %d %d", &periodic_zap_period,
            &periodic_zap_phase, &periodic_zap_width);

        if (rv<2 || rv>3)
	  throw Error (InvalidState, "paz::parse",
		       "Invalid parameter to option -p");
        else if (rv==2) 
          periodic_zap_width = 1;

	command += " -p ";
	command += c_arg;
	break;
      }

    case 'Z':
      {
	unsigned first = 1;
	unsigned last = 0;
	char delimiter = 0;

	zero_channels = true;
	if (sscanf (c_arg, "%u%c%u", &first, &delimiter, &last) != 3)
	  throw Error (InvalidState, "paz::parse",
		       " '-Z %s' not understood", c_arg);

	for (unsigned i = first; i <= last; i++)
	  chans_to_zap.push_back (i);
      }
      command += " -Z ";
      command += c_arg;
      break;

    case 'F':
      {
        FreqRange range;
        zero_channels= true;
        if (sscanf (c_arg, "%lf %lf", &range.lo, &range.hi) !=2)
	  throw Error (InvalidState, "paz::parse",
		       "Invalid parameter to option -F");

        if (range.lo > range.hi)
        { 
          double tmp = range.lo;
          range.lo = range.hi;
          range.hi = tmp;
        }
        freq_ranges_to_zap.push_back(range);
      }
      command += " -F ";
      command += c_arg;
      break;

    case 'x':
      command += " -x ";
      command += c_arg;
      key = strtok (c_arg, whitespace);
      nozap_subints = true;
      while (key) {
	if (sscanf (key, "%d", &placeholder) == 1) {
	  subs_nozap.push_back (placeholder);
	}
	key = strtok (NULL, whitespace);
      }
      break;

    case 'X':
      {
	unsigned first = 1;
	unsigned last = 0;

	nozap_subints = true;
	if (sscanf (c_arg, "%d %d", &first, &last) != 2)
	  throw Error (InvalidState, "paz::parse",
		       "Invalid parameter to option -X");

	for (unsigned i = first; i <= last; i++)
	  subs_nozap.push_back (i);
      }
      command += " -X ";
      command += c_arg;
      break;

    case 'd':
      simple = true;
      command += " -d";
      break;

    case 'b':
      if (!median_zapper)
	median_zapper = new Pulsar::ChannelZapMedian;
      median_zapper->set_bybin (true);
      command += " -b";
      break;

    case 'r':
      if (!median_zapper)
	median_zapper = new Pulsar::ChannelZapMedian;
      command += " -r";
      break;

    case 'R':{
	unsigned window = atoi (c_arg);

	if (!median_zapper)
	  median_zapper = new Pulsar::ChannelZapMedian;

	median_zapper->set_window_size (window);
	command += " -R ";
	command += c_arg;
	break;
      }

    case 'u':
      ulpath = c_arg;
      if (ulpath.substr (ulpath.length () - 1, 1) != "/")
	ulpath += "/";
      command += " -u ";
      command += c_arg;
      break;

    case 'E':
      edge_zap = true;
      if (sscanf (c_arg, "%f", &percent) != 1)
	  throw Error (InvalidState, "paz::parse",
		       "Invalid parameter to option -E");

      if (percent <= 0.0 || percent >= 100.0)
	  throw Error (InvalidState, "paz::parse",
		       "Invalid parameter to option -E");

      command += " -E ";
      command += c_arg;
      break;

    case 's':
      command += " -s ";
      command += c_arg;
      key = strtok (c_arg, whitespace);
      zap_subints = true;
      while (key) {
	if (sscanf (key, "%d", &placeholder) == 1) {
	  subs_to_zap.push_back (placeholder);
	}
	key = strtok (NULL, whitespace);
      }
      break;

    case 'S':
      {
	unsigned first = 1;
	unsigned last = 0;

	zap_subints = true;
	if (sscanf (c_arg, "%d %d", &first, &last) != 2)
	  throw Error (InvalidState, "paz::parse",
		       "Invalid parameter to option -S");

	for (unsigned i = first; i <= last; i++)
	  subs_to_zap.push_back (i);
      }
      command += " -S ";
      command += c_arg;
      break;

    case 'w':
      command += " -w ";
      command += c_arg;
      key = strtok (c_arg, whitespace);
      zero_subints = true;
      while (key) {
	if (sscanf (key, "%d", &placeholder) == 1) {
	  subs_to_zap.push_back (placeholder);
	}
	key = strtok (NULL, whitespace);
      }
      break;

    case 'W':
      {
	unsigned first = 1;
	unsigned last = 0;

	zero_subints = true;

	if (sscanf (c_arg, "%d %d", &first, &last) != 2)
	  throw Error (InvalidState, "paz::parse",
		       "Invalid parameter to option -W");

	for (unsigned i = first; i <= last; i++)
	  subs_to_zap.push_back (i);
      }
      command += " -W ";
      command += c_arg;
      break;

    case 'C':
      zap_ston = true;
      if (sscanf (c_arg, "%lf", &ston_cutoff) != 1)
	  throw Error (InvalidState, "paz::parse",
		       "Invalid parameter to option -C");

      command += " -C ";
      command += c_arg;
      break;

    case 'o':
      dropout_zap = true;
      if (sscanf (c_arg, "%f", &dropout_sigma) != 1)
	  throw Error (InvalidState, "paz::parse",
		       "Invalid parameter to option -o");

      command += " -o ";
      command += c_arg;
      break;

    case 'P':
      try {
	Reference::To < Pulsar::Archive > data =
	  Pulsar::Archive::load (c_arg);
	data->pscrunch ();
	data->fscrunch ();
	data->tscrunch ();
	thestd = data->get_Profile (0, 0, 0);
	std_given = true;

	standard_snr.set_standard (thestd);

	Pulsar::Profile::snr_strategy.set (&standard_snr,
					   &Pulsar::StandardSNR::get_snr);

      }
      catch (Error & error) {
	cout << error << endl;
	cout << "Could not load given standard profile" << endl;
	std_given = false;
      }
      command += " -P ";
      command += c_arg;
      break;

    default:
      return false;
    }

  return true;
}

void paz::setup ()
{
  if (zero_intersection && !(zero_channels && zero_subints))
    throw Error (InvalidState, "paz::setup",
		 "cannot use -I with both -s|S|k *and* -w|W");
}

// Instantiate an RFIMitigation class
Pulsar::RFIMitigation* zapper = new Pulsar::RFIMitigation;

void paz::process (Pulsar::Archive* arch)
{
  if (periodic_zap)
  {
    if (arch->get_nchan () == 1) {
      printf
	("Warning! Periodic spike zapping on frequency-scrunched dedispersed data may\n");
      printf ("not be what you intended! (spikes will wash out)\n");
    }
    for (unsigned pol = 0; pol < arch->get_npol (); pol++)
      for (unsigned chan = 0; chan < arch->get_nchan (); chan++)
	for (unsigned subint = 0; subint < arch->get_nsubint (); subint++)
	  zap_periodic_spikes (arch->get_Profile (subint, pol, chan),
			       periodic_zap_period, periodic_zap_phase,
                               periodic_zap_width);
  }

  if (eightBinZap) {		// To fix early wide-band correlator problem

    for (unsigned pol = 0; pol < arch->get_npol (); pol++) {
      for (unsigned chan = 0; chan < arch->get_nchan (); chan++) {
	for (unsigned subint = 0; subint < arch->get_nsubint (); subint++) {
	  Pulsar::Profile * prof = arch->get_Profile (subint, pol, chan);
	  float *amps = prof->get_amps ();
	  for (unsigned i = 5; i < arch->get_nbin (); i += 8) {
	    // Interpolate between previous and later point if possible
	    if (i == arch->get_nbin () - 1)
	      amps[i] = amps[i - 1];
	    else {
	      float amp1 = amps[i - 1];
	      float amp2 = amps[i + 1];
	      amps[i] = 0.5 * (amp1 + amp2);
	    }
	  }
	  prof->set_amps (amps);
	}
      }
    }
  }

  unsigned nchan = arch->get_nchan ();
  double chan_bw = fabs(arch->get_bandwidth() / (double)nchan);

  if (zap_subints)
  {
    unsigned zapped = 0;
    unsigned orig_nsubint = arch->get_nsubint ();

    for (unsigned i = 0; i < orig_nsubint; i++)
      for (unsigned j = 0; j < subs_to_zap.size (); j++)
	if (subs_to_zap[j] == i)
	{
	  if (verbose)
	    cout << "Zapping subint " << i << endl;

	  // after each subint is zapped, it is necessary to offset the index
	  arch->erase (i - zapped);
	  zapped ++;
	}

    if (verbose)
      cerr << "paz: extraction completed" << endl;
  }

  if (nozap_subints)
  {
    unsigned zapped = 0;
    unsigned orig_nsubint = arch->get_nsubint ();

    for (unsigned i = 0; i < orig_nsubint; i++)
    {
      bool keep = false;
      for (unsigned j = 0; j < subs_nozap.size (); j++)
	if (subs_nozap[j] == i)
	  keep = true;

      if (!keep)
      {
	if (verbose)
	  cout << "Zapping subint " << i << endl;

	// after each subint is zapped, it is necessary to offset the index
	arch->erase (i - zapped);
	zapped ++;
      }
    }
  }

  if (median_zapper)
    {
      cout << "Using median smoothed difference zapper" << endl;
      (*median_zapper) (arch);
    }

  if (modulation_zapper)
    {
      cout << "Using modulation index zapper" << endl;
      (*modulation_zapper) (arch);
    }

  if (simple)
    {
      cout << "Using simple mean offset zapper" << endl;
      zapper->zap_chans (arch);
      cout << "Zapping complete" << endl;
    }

  if (zero_subints && !zero_intersection)
    {
      vector < float >mask (nchan, 0.0);
      zapper->zap_very_specific (arch, mask, subs_to_zap);
    }

  if (bin_zap && bins_to_zap.size ())
    {
      for (unsigned i = 0; i + 5 <= bins_to_zap.size (); i += 5)
	binzap (arch, arch->get_Integration (bins_to_zap[i]), bins_to_zap[i],
		bins_to_zap[i + 1], bins_to_zap[i + 2], bins_to_zap[i + 3],
		bins_to_zap[i + 4]);
    }

  if (zero_channels)
    {
      if (!killfile.empty ()) {
	vector < int >some_chans;
	char *useful = new char[4096];
	FILE *fptr = fopen (killfile.c_str (), "r");

	while (fgets (useful, 4096, fptr)) {
	  key = strtok (useful, whitespace);
	  while (key) {
	    if (sscanf (key, "%d", &placeholder) == 1) {
	      some_chans.push_back (placeholder);
	    }
	    key = strtok (NULL, whitespace);
	  }
	  for (unsigned x = 0; x < some_chans.size (); x++) {
	    chans_to_zap.push_back (some_chans[x]);
	  }

	  some_chans.resize (0);
	}
	delete[]useful;
      }

      vector < float >mask (nchan, 1.0);
      for (unsigned i = 0; i < chans_to_zap.size (); i++) {
	mask[chans_to_zap[i]] = 0.0;
      }

      for (unsigned i = 0; i < freqs_to_zap.size (); i++) {
        for (unsigned ic=0; ic<nchan; ic++) {
          double chan_freq=arch->get_Integration(0)->get_centre_frequency(ic);
          if ( freqs_to_zap[i] > chan_freq - chan_bw/2.0 
	       && freqs_to_zap[i] < chan_freq + chan_bw/2.0 )
            mask[ic] = 0.0;
        }
      }

      for (unsigned i=0; i< freq_ranges_to_zap.size(); i++) {
        for (unsigned ic=0; ic<nchan; ic++) {
          FreqRange chan;
          chan.lo=arch->get_Integration(0)->get_centre_frequency(ic) 
            - chan_bw/2.0;
          chan.hi=arch->get_Integration(0)->get_centre_frequency(ic) 
            + chan_bw/2.0;
          if (chan.lo > freq_ranges_to_zap[i].lo 
              && chan.lo < freq_ranges_to_zap[i].hi)
            mask[ic] = 0.0;
          else if (chan.hi > freq_ranges_to_zap[i].lo 
		   && chan.hi < freq_ranges_to_zap[i].hi)
            mask[ic] = 0.0;
          else if (freq_ranges_to_zap[i].lo > chan.lo 
		   && freq_ranges_to_zap[i].lo < chan.hi)
            mask[ic] = 0.0;
        }
      }

      if (zero_intersection)
	zapper->zap_very_specific (arch, mask, subs_to_zap);
      else
	zapper->zap_specific (arch, mask);
    }

  if (edge_zap)
    {
      float fraction = percent / 100.0;
      unsigned buffer = unsigned (float (nchan) * fraction);

      vector < float >mask (nchan, 0.0);

      for (unsigned i = buffer; i < (nchan - buffer); i++)
	mask[i] = 1.0;

      zapper->zap_specific (arch, mask);
    }

  if (zap_ston)
    {
      double theston = 0.0;
      Reference::To<Pulsar::Archive> cloned = arch->clone();
      cloned->pscrunch ();
      for (unsigned isub = 0; isub < arch->get_nsubint (); isub++) {
	for (unsigned ichan = 0; ichan < arch->get_nchan (); ichan++) {
	  theston = cloned->get_Profile (isub, 0, ichan)->snr ();
	  if (theston < ston_cutoff) {
	    arch->get_Integration (isub)->set_weight (ichan, 0.0);
	  }
	}
      }
    }

  if (dropout_zap) {
    Reference::To < Pulsar::Archive > cloned = arch->clone ();
    Reference::To < Pulsar::Profile > testprof = 0;

    cloned->pscrunch ();
    cloned->fscrunch ();
    cloned->remove_baseline ();

    vector < double >mins;
    for (unsigned isub = 0; isub < arch->get_nsubint (); isub++) {
      testprof = cloned->get_Profile (isub, 0, 0);
      mins.push_back (fabs (testprof->min ()));
    }

    cloned->tscrunch ();

    double mean, vari, varm;

    testprof = cloned->get_Profile (0, 0, 0);
    testprof->stats (&mean, &vari, &varm);

    for (unsigned isub = 0; isub < arch->get_nsubint (); isub++) {
      if (mins[isub] > dropout_sigma * sqrt (vari)) {
	cerr << "Zapping integration " << isub << endl;
	arch->get_Integration (isub)->uniform_weight (0.0);
      }
    }
  }

  if (mower)
  {
    vector < unsigned > mowing;

    if (subints_to_mow.size ())
      mowing = subints_to_mow;
    else
    {
      mowing.resize (arch->get_nsubint ());
      for (unsigned isub = 0; isub < arch->get_nsubint (); isub++)
	mowing[isub] = isub;
    }

    for (unsigned isub = 0; isub < mowing.size (); isub++)
    {
      cerr << "paz: mowing subint " << mowing[isub] << endl;
      mower->transform (arch->get_Integration (mowing[isub]));
    }
  }

  /////////////////////////////////////////////////////////////////
    // See if the archive contains a history that should be updated:
    
    Pulsar::ProcHistory * fitsext = arch->get < Pulsar::ProcHistory > ();

    if (fitsext) {

      if (command.length () > 80) {
	cout << "WARNING: ProcHistory command string truncated to 80 chars"
	     << endl;
	fitsext->set_command_str (command.substr (0, 80));
      } else {
	fitsext->set_command_str (command);
      }

    }
}

void
binzap (Pulsar::Archive * arch, Pulsar::Integration * integ, int subint,
	int lower_range, int upper_range, int lower_bin, int upper_bin)
{
  BoxMuller gasdev;
  float mean;
  float deviation;
  float *this_int;

  for (unsigned i = 0; i < arch->get_npol (); i++) {
    int j;
    for (unsigned k = 0; k < arch->get_nchan (); k++) {
      this_int = integ->get_Profile (i, k)->get_amps ();
      mean = 0;
      deviation = 0;

      // calculate mean across the bins, excluding bins to be zapped

      for (j = lower_range; j < lower_bin; j++)
	mean += this_int[j];

      for (j = upper_bin; j < upper_range; j++)
	mean += this_int[j];

      mean = mean / ((upper_range - lower_range) - (upper_bin - lower_bin));

      for (j = lower_range; j < lower_bin; j++)
	deviation += (this_int[j] - mean) * (this_int[j] - mean);

      for (j = upper_bin; j < upper_range; j++)
	deviation += (this_int[j] - mean) * (this_int[j] - mean);

      deviation =
	deviation / ((upper_range - lower_range) -
		     (upper_bin - lower_bin - 1));
      deviation = sqrt (deviation);

      // assign a new value based on the mean, random Gaussian number
      // and variance to the bins being zapped

      for (j = lower_bin; j < upper_bin; j++)
	this_int[j] = mean + (gasdev () * deviation);
    }
  }
}
