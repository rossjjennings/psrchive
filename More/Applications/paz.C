/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
// A simple command line tool for zapping RFI

#include "Pulsar/psrchive.h"

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

using namespace std;

void
usage ()
{
  cout << "A program for zapping RFI in Pulsar::Archives. Changes\n"
    "will be noted in the history of PSRFITS files.\n"
    "Usage: paz [options] filenames\n"
    "  -h               This help page\n"
    "  -v               Verbose mode\n"
    "  -V               Very verbose mode\n"
    "  -i               Show revision information\n"
    "\n"
    "File unloading options:\n"
    "  -m               Modify the original files on disk\n"
    "  -e               Unload to new files using this extension\n"
    "  -u path          Write new files to this location\n"
    "\n"
    "Manual zapping options:\n"
    "  -I               Zero weight the intersection of -[wW] and -[zZk]\n"
    "  -k filename      Zero weight chans listed in this kill file\n"
    "  -z \"a b c ...\"   Zero weight these particular channels\n"
    "  -Z \"a b\"         Zero weight chans between a & b inclusive\n"
    "  -f \"a b c ...\"   Zero weight channels with these frequencies (MHz)\n"
    "  -x \"a b c ...\"   Delete all sub-integrations except these\n"
    "  -X \"a b\"         Delete all sub-ints except a to b inclusive\n"
    "  -E percent       Zero weight this much of each band edge\n"
    "  -s \"a b c ...\"   Delete these sub-integrations\n"
    "  -S \"a b\"         Delete sub-ints between a & b inclusive\n"
    "  -w \"a b c ...\"   Zero weight these sub-integrations\n"
    "  -W \"a b\"         Zero weight sub-ints between a & b inclusive\n"
    "  -p \"p i\"         Interpolate over every p-th phase bin, starting at i\n"
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
    "  -8               Attempts to fix ATNF WBCORR 8 bin problem (see also -p)\n"
    "\n"
    "The format of the kill file used with the -k option is simply\n"
    "a list of channel numbers, separated by white space\n"
    "\n"
    "The cutoff S/N value used with -C is largely arbitrary. You will\n"
    "need to experiment to find the best value for your archives\n"
    "\n" "See " PSRCHIVE_HTTP "/manuals/paz for more details\n" << endl;
}

void zap_periodic_spikes (Pulsar::Profile * profile, int period, int phase);
void binzap (Pulsar::Archive * arch, Pulsar::Integration * integ, int subint,
	     int lower_bin, int upper_bin, int lower_range, int upper_range);

int
main (int argc, char *argv[])
{

  bool bin_zap = false;
  vector < int >bins_to_zap;

  bool verbose = false;
  bool write = false;
  bool eightBinZap = false;
  vector < string > archives;

  string ulpath;

  string killfile;

  char *metafile = NULL;

  bool zero_channels = false;
  vector < int >chans_to_zap;
  vector < double >freqs_to_zap;

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
  int periodic_zap_period = 8, periodic_zap_phase = 0;

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

  const char *args = "8bC:B:dDe:E:f:hIik:l:LmM:no:p:P:rR:s:S:u:vVw:W:x:X:z:Z:";

  string command = "paz";

  while ((gotc = getopt (argc, argv, args)) != -1) {
    switch (gotc) {
    case 'h':
      usage ();
      return -1;
      break;
    case 'v':
      verbose = true;
      break;
    case 'V':
      verbose = true;
      Pulsar::Archive::set_verbosity (3);
      break;
    case 'i':
      cout << "$Id: paz.C,v 1.47 2008/07/31 01:39:17 demorest Exp $" << endl;
      return 0;

    case 'm':
      write = true;
      command += " -m";
      break;

    case 'M':
      metafile = optarg;
      break;

    case 'I':
      zero_intersection = true;
      command += " -I";
      break;

    case 'k':
      killfile = optarg;
      zero_channels = true;
      command += " -k ";
      command += optarg;
      break;

    case 'l':
      command += " l ";
      command += optarg;
      subints_to_mow.push_back (atoi (optarg));
      mower = new Pulsar::LawnMower;
      break;
    case 'L':
      command += " -L ";
      mower = new Pulsar::LawnMower;
      break;

    case 'z':
      command += " -z ";
      command += optarg;
      key = strtok (optarg, whitespace);
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
      command += optarg;
      key = strtok (optarg, whitespace);
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
      command += optarg;
      key = strtok (optarg, whitespace);
      bin_zap = true;
      while (key) {
	if (sscanf (key, "%d", &placeholder) == 1) {
	  bins_to_zap.push_back (placeholder);
	}
	key = strtok (NULL, whitespace);
      }
      break;

    case 'e':
      write = true;
      ext = optarg;
      command += " -e ";
      command += optarg;
      break;

    case '8':
      // The aim here is to zero every eight time bins starting from number 6
      eightBinZap = true;
      command += " -8";
      break;

    case 'p':
      {
	periodic_zap = true;

	if (sscanf (optarg, "%d %d", &periodic_zap_period,
		    &periodic_zap_phase) != 2) {
	  cerr << "Invalid parameter to option -Z" << endl;
	  return (-1);
	}
	command += " -p ";
	command += optarg;
	break;
      }

    case 'Z':
      {
	unsigned first = 1;
	unsigned last = 0;

	zero_channels = true;
	if (sscanf (optarg, "%d %d", &first, &last) != 2) {
	  cerr << "Invalid parameter to option -Z" << endl;
	  return (-1);
	}

	for (unsigned i = first; i <= last; i++)
	  chans_to_zap.push_back (i);
      }
      command += " -Z ";
      command += optarg;
      break;

    case 'x':
      command += " -x ";
      command += optarg;
      key = strtok (optarg, whitespace);
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
	if (sscanf (optarg, "%d %d", &first, &last) != 2) {
	  cerr << "Invalid parameter to option -X" << endl;
	  return (-1);
	}

	for (unsigned i = first; i <= last; i++)
	  subs_nozap.push_back (i);
      }
      command += " -X ";
      command += optarg;
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
	unsigned window = atoi (optarg);

	if (!median_zapper)
	  median_zapper = new Pulsar::ChannelZapMedian;

	median_zapper->set_window_size (window);
	command += " -R ";
	command += optarg;
	break;
      }

    case 'u':
      ulpath = optarg;
      if (ulpath.substr (ulpath.length () - 1, 1) != "/")
	ulpath += "/";
      command += " -u ";
      command += optarg;
      break;

    case 'E':
      edge_zap = true;
      if (sscanf (optarg, "%f", &percent) != 1) {
	cerr << "Invalid parameter to option -e" << endl;
	return (-1);
      }
      if (percent <= 0.0 || percent >= 100.0) {
	cerr << "Invalid parameter to option -e" << endl;
	return (-1);
      }
      command += " -E ";
      command += optarg;
      break;

    case 's':
      command += " -s ";
      command += optarg;
      key = strtok (optarg, whitespace);
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
	if (sscanf (optarg, "%d %d", &first, &last) != 2) {
	  cerr << "Invalid parameter to option -S" << endl;
	  return (-1);
	}

	for (unsigned i = first; i <= last; i++)
	  subs_to_zap.push_back (i);
      }
      command += " -S ";
      command += optarg;
      break;

    case 'w':
      command += " -w ";
      command += optarg;
      key = strtok (optarg, whitespace);
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

	if (sscanf (optarg, "%d %d", &first, &last) != 2) {
	  cerr << "Invalid parameter to option -W" << endl;
	  return (-1);
	}

	for (unsigned i = first; i <= last; i++)
	  subs_to_zap.push_back (i);
      }
      command += " -W ";
      command += optarg;
      break;

    case 'C':
      zap_ston = true;
      if (sscanf (optarg, "%lf", &ston_cutoff) != 1) {
	cerr << "Invalid parameter to option -S" << endl;
	return (-1);
      }
      command += " -C ";
      command += optarg;
      break;

    case 'o':
      dropout_zap = true;
      if (sscanf (optarg, "%f", &dropout_sigma) != 1) {
	cerr << "Invalid parameter to option -o" << endl;
	return (-1);
      }
      command += " -o ";
      command += optarg;
      break;

    case 'P':
      try {
	Reference::To < Pulsar::Archive > data =
	  Pulsar::Archive::load (optarg);
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
      command += optarg;
      break;

    default:
      cout << "Unrecognised option" << endl;
    }
  }

  if (zero_intersection && !(zero_channels && zero_subints)) {
    cerr << "paz: cannot use -I with both -s|S|k *and* -w|W" << endl;
    return -1;
  }

  if (metafile) 
    stringfload (&archives, metafile);
  else 
    for (int ai = optind; ai < argc; ai++)
      dirglob (&archives, argv[ai]);

  if (archives.empty ()) {
    cerr << "No archives were specified" << endl;
    exit (-1);
  }

  if (!write)
    cout << "Changes will not be saved. Use -m or -e to write results to disk"
      << endl;

  // Instantiate an RFIMitigation class

  Pulsar::RFIMitigation * zapper = new Pulsar::RFIMitigation;

  // Start zapping archives

  for (unsigned i = 0; i < archives.size (); i++)
    try {

    if (verbose)
      cerr << "Loading " << archives[i] << endl;

    Reference::To < Pulsar::Archive > arch =
      Pulsar::Archive::load (archives[i]);

    cout << "Loaded archive: " << archives[i] << endl;


    if (periodic_zap) {
      if (arch->get_nchan () == 1) {
	printf
	  ("Warning! Periodic spike zapping on frequency-scrunched dedispersed data may\n");
	printf ("not be what you intended! (spikes will wash out)\n");
      }
      for (unsigned pol = 0; pol < arch->get_npol (); pol++)
	for (unsigned chan = 0; chan < arch->get_nchan (); chan++)
	  for (unsigned subint = 0; subint < arch->get_nsubint (); subint++)
	    zap_periodic_spikes (arch->get_Profile (subint, pol, chan),
				 periodic_zap_period, periodic_zap_phase);
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

    int nchan = arch->get_nchan ();
    double chan_bw = fabs(arch->get_bandwidth() / (double)nchan);

    if (zap_subints) {

      Reference::To < Pulsar::Archive > new_arch;

      vector < unsigned >subs_to_keep;
      bool ignore;

      for (unsigned i = 0; i < arch->get_nsubint (); i++) {
	ignore = false;
	for (unsigned j = 0; j < subs_to_zap.size (); j++)
	  if (subs_to_zap[j] == i) {
	    ignore = true;
	    if (verbose)
	      cout << "Zapping subint " << i << endl;
	  }
	if (!ignore) {
	  subs_to_keep.push_back (i);
	}
      }
      new_arch = arch->extract (subs_to_keep);
      string useful = arch->get_filename ();
      arch = new_arch;
      arch->set_filename (useful);
    }

    if (nozap_subints) {

      Reference::To < Pulsar::Archive > new_arch;

      vector < unsigned >subs_to_keep;
      bool keep;

      for (unsigned i = 0; i < arch->get_nsubint (); i++) {
	keep = false;
	for (unsigned j = 0; j < subs_nozap.size (); j++)
	  if (subs_nozap[j] == i) {
	    keep = true;
	  }
	if (keep) {
	  subs_to_keep.push_back (i);
	  if (verbose)
	    cout << "Keeping subint " << i << endl;
	}
      }
      new_arch = arch->extract (subs_to_keep);
      string useful = arch->get_filename ();
      arch = new_arch->clone ();
      arch->set_filename (useful);
    }

    if (median_zapper) {
      cout << "Using median smoothed difference zapper" << endl;
      median_zapper->ChannelWeight::weight (arch);
    }

    if (modulation_zapper) {
      cout << "Using modulation index zapper" << endl;
      modulation_zapper->ChannelWeight::weight (arch);
    }

    if (simple) {
      cout << "Using simple mean offset zapper" << endl;
      zapper->zap_chans (arch);
      cout << "Zapping complete" << endl;
    }

    if (zero_subints && !zero_intersection) {
      vector < float >mask (nchan, 0.0);
      zapper->zap_very_specific (arch, mask, subs_to_zap);
    }

    if (bin_zap && bins_to_zap.size ()) {
      for (unsigned i = 0; i + 5 <= bins_to_zap.size (); i += 5)
	binzap (arch, arch->get_Integration (bins_to_zap[i]), bins_to_zap[i],
		bins_to_zap[i + 1], bins_to_zap[i + 2], bins_to_zap[i + 3],
		bins_to_zap[i + 4]);
    }

    if (zero_channels) {
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

      if (zero_intersection)
	zapper->zap_very_specific (arch, mask, subs_to_zap);
      else
	zapper->zap_specific (arch, mask);
    }

    if (edge_zap) {
      float fraction = percent / 100.0;
      int buffer = int (float (nchan) * fraction);

      vector < float >mask (nchan, 0.0);

      for (int i = (0 + buffer); i <= (nchan - buffer); i++) {
	mask[i] = 1.0;
      }
      zapper->zap_specific (arch, mask);
    }

    if (zap_ston) {
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

    if (mower) {

      vector < unsigned >mowing;

      if (subints_to_mow.size ())
	mowing = subints_to_mow;
      else {
	mowing.resize (arch->get_nsubint ());
	for (unsigned isub = 0; isub < arch->get_nsubint (); isub++)
	  mowing[isub] = isub;
      }

      for (unsigned isub = 0; isub < mowing.size (); isub++) {
	cerr << "paz: mowing subint " << mowing[isub] << endl;
	mower->transform (arch->get_Integration (mowing[isub]));
      }

    }

    if (!write)
      continue;

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

    if (ext.empty ()) {
      cout << "Unloading " << arch->get_filename () << " ..." << endl;
      arch->unload ();
      cout << arch->get_filename () << " updated on disk" << endl;
      continue;
    }

    string the_new = replace_extension (arch->get_filename (), ext);
    if (!ulpath.empty ())
      the_new = ulpath + the_new;

    cout << "Unloading " << the_new << " ..." << endl;
    arch->unload (the_new);
    cout << "New file " << the_new << " written to disk" << endl;

    }
  catch (Error & error) {
    cerr << "paz: Error while handling " << archives[i] << error << endl;
  }

  return 0;
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
