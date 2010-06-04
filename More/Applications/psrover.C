/***************************************************************************
 *
 *   Copyright (C) 2009,2010 by Stefan Oslowski,Jonathan Khoo
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"

#include <string.h>
#include <time.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

#include "BoxMuller.h"
#include <dirutil.h>
#include "Pulsar/psrchive.h"
#include "Pulsar/Archive.h"
#include "Pulsar/ArchiveExpert.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Integration.h"

#include <algorithm>

#include <boost/tokenizer.hpp>

#define sub_bin 10

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::vector;

using Pulsar::Archive;

class psrover : public Pulsar::Application
{
 public:
  //! Default constructor:
  psrover ();

  //! Main loop
  //  void run ();

  //! Process the given archive
  void process (Pulsar::Archive*);

  //! Initial setup
  void setup ();

  //!
  // void finalize ();

 protected:

  void add_options (CommandLine::Menu&);
  void set_noise (string);
  void set_fwhms (string);
  void set_bins (string);
  void set_ascii_file (string);
  void set_nbins (int);
  void set_output_filename (string);

  time_t seconds;
  BoxMuller gasdev;

  string ascii_filename;
  string output_filename;

  vector<float> noise_to_add;
  vector<float> fwhms;
  vector<float> bins;
  vector<float> profile_values;
  

  float current_bin;
  float tmp_rand;

  int nbin;

  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

  bool got_noises;
  bool got_fwhms;
  bool got_bins;
  bool got_ascii_file;
  bool got_nbins;
};

psrover::psrover()
	: Application ("psrover", "program for adding noise to data")
{
  Pulsar::StandardOptions* preprocessor = new Pulsar::StandardOptions;
  add (preprocessor );

  seconds = time(NULL);
  gasdev = BoxMuller(seconds);

  got_nbins = got_ascii_file = got_noises = got_fwhms = got_bins = false;

  output_filename = "temp_archive.ar";
}

void psrover::add_options ( CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  menu.add("");

  arg = menu.add(this, &psrover::set_noise,'r',"noise amplitudes");
  arg->set_help ("coma separated list of noise amplitudes to be added");

  arg = menu.add(this, &psrover::set_fwhms,'f',"fwhms");
  arg->set_help ("coma separated list of FWHM of gaussian peaks");
  arg->set_long_help ("This option will require also to set the centre bins\n"
		  "Negative value means that the corresponding noise amplitude will be the value of delta function\n"
		  "at the corresponding bin, or gaussian noise, in case the bin is negative as well\n");

  arg = menu.add(this, &psrover::set_bins,'b',"bins");
  arg->set_help ("coma separated list of centre bins for gaussian peaks");
  arg->set_help ("negative value means that the corresponding noise amplitude is for random noise");

  arg = menu.add(this, &psrover::set_ascii_file,'a',"ascii file");
  arg->set_help ("ascii file with desired underlying profile");
  arg->set_long_help ("if given, it will overwrite the data from archive");

  arg = menu.add(this, &psrover::set_ascii_file,'n',"number of bins");
  arg->set_help ("the number of bins in the requested profile");
  arg->set_long_help("This will be overriden by the ascii file or archive, if any given");

  arg = menu.add(this, &psrover::set_output_filename,'o',"output file");
  arg->set_help ("name of the output, defaults to temp_archive.ar");
}

void psrover::set_output_filename (string _output) {
  output_filename = _output;
}

void psrover::set_nbins (int _nbins) {
  nbin = _nbins;
  got_nbins = true;
}

void psrover::set_noise(string _noise) {
  boost::char_separator<char> sep(",");
  tokenizer tokens(_noise, sep);
  tokenizer::iterator tok_iter;
  for (tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
    noise_to_add.push_back(fromstring<float>(*tok_iter));
  got_noises = true;
}

void psrover::set_fwhms(string _fwhms) {
  boost::char_separator<char> sep(",");
  tokenizer tokens(_fwhms, sep);
  tokenizer::iterator tok_iter;
  for (tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
    fwhms.push_back(fromstring<float>(*tok_iter));
  got_fwhms = true;
}

void psrover::set_bins(string _bins) {
  boost::char_separator<char> sep(",");
  tokenizer tokens(_bins, sep);
  tokenizer::iterator tok_iter;
  for (tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
    bins.push_back(fromstring<float>(*tok_iter));
  got_bins = true;
}

void psrover::set_ascii_file (string _file) {
  if (verbose)
    cerr << "psrover::set_ascii_file setting to " << _file << endl;
  ascii_filename = _file;
  got_ascii_file = true;
}

/*void psrover::run ()
{
  if (!filenames.empty())
    Application::run ();
}*/

void psrover::setup () {
  if (got_ascii_file) {
    if (verbose)
      cerr << "psrover::setup reading the ascii file" << endl;
    ifstream in_file(ascii_filename.c_str());
    if (in_file.is_open()) {
      while (!in_file.eof()) {
	float f;
	in_file >> f;
	profile_values.push_back(f);
	nbin ++;
      }
      in_file.close();
    } 
  }
  else {
    if (verbose)
      cerr << "psrover::setup defaulting to an empty profile" << endl;
    if (!got_nbins) {
      if (verbose)
       cerr << "psrover::setup no nbins provided, defaulting to 1024" << endl;
      nbin = 1024;
    }
    for (int i = 0 ; i < nbin ; i++) {
      profile_values.push_back(0.0);
    }
  }
}

void psrover::process (Pulsar::Archive* archive) {
  nbin = archive-> get_nbin();

  if (verbose)
    cerr << "psrover::process assigning data from " << archive->get_filename() << endl;

  float* data = archive->get_Integration(0)->get_Profile(0, 0)->get_amps();

  if (noise_to_add[0] != 0.0) {
    for (unsigned i = 0; i < nbin; ++i) {
      if (got_ascii_file)
	*data = profile_values[i];

      //generate random number and use it as a multiplicand with the noise input
      for (int j = 0; j < fwhms.size(); j++ ) {
	if (bins[j] < 0 && fwhms[j] < 0) {
	  if (i == 0 ) 
	    cout << "Adding " << noise_to_add[j] << " noise to each bin" << endl;
	  *data += gasdev() * (float)noise_to_add[j];
	}
	if (fwhms[j] < 0 && bins[j] > 0 && i == bins[j]) {
	  *data += noise_to_add[j];
	}
      }
      ++data;
    }
    data -= 1024;
    for (int j = 0; j < fwhms.size(); j++ ) {
      if (fwhms[j] > 0 && bins[j] > 0) {
	cout << "Adding gaussian noise:" << endl;
	cout << "maximum: " << noise_to_add[j] << " peak at the bin: " << bins[j] << " fwhm: " << fwhms[j] << endl;
	for (unsigned i = 0; i < nbin; ++i) {
	  // resolve the gaussian with sub_bin resolution:
	  for (int k = 0; k < sub_bin; k++ ) {
	    current_bin = (float)((signed)i) + 1.0 / sub_bin * (float)k;
	    *data += noise_to_add[j] / sub_bin * exp( -pow((signed)i-bins[j],2) / 2 / pow(fwhms[j] / 2.35482, 2)) ;
	  }
	  ++data;
	}
      } 
    }
  } else {
    for (unsigned i = 0; i < nbin; ++i) {
      *data = profile_values[i];
      ++data;
    }
  }

  if (verbose)
    cerr << "Unloading " << endl;
  archive->unload(output_filename);
}

int main (int argc, char** argv) 
{
  psrover program;
  return program.main (argc, argv);
}
