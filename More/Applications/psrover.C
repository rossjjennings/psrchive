/***************************************************************************
 *
 *   Copyright (C) 2009 - 2013 by Stefan Oslowski,Jonathan Khoo
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// TODO Unexpected behaviour when von Mises component specified before white noise

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"

#include <string.h>
#include <time.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "BoxMuller.h"
#include "Pulsar/ComponentModel.h"
#include "Pulsar/psrchive.h"
#include "Pulsar/Archive.h"
#include "Pulsar/ArchiveExpert.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Integration.h"


#define sub_bin 10

using namespace std;
using namespace Pulsar;

class psrover : public Pulsar::Application
{
 public:
  //! Default constructor:
  psrover ();

  //! Main loop
  //  void run ();

  //! Process the given archive
  void process (Pulsar::Archive*);

  //! Over-write the input archive with simulated data
  void over (Pulsar::Archive*);

  //! Initial setup
  void setup ();

  //! Handle all the final actions
  void finalize ();

 protected:

  void add_options (CommandLine::Menu&);
  void set_noise (string);
  void set_fwhms (string);
  void set_bins (string);
  void set_ascii_file (string);
  void set_nbins (int);

  void initialise_basis ( float*, unsigned );

  unsigned nfiles;

  time_t seconds;
  long seed;
  BoxMuller gasdev;
  Reference::To<ComponentModel> model;

  string ascii_filename;
  string output_filename;
  string out_path;
  long file_count;
  bool sequential_files;
  std::stringstream ss;

  string save_added_noise_filename;
  string save_added_gauss_filename;
  string save_profile_ascii;
  ofstream outfile1;
  ofstream outfile2;
  ofstream outfile3;

  vector<double> noise_to_add;
  vector<double> fwhms;
  vector<double> bins;
  vector<double> profile_values;
  

  float current_bin;
  float tmp_rand;
  float amplitude;

  unsigned nbin;
  unsigned i_pol;
  unsigned i_chan;

  bool got_noises;
  bool got_fwhms;
  bool got_bins;
  bool got_ascii_file;
  bool got_nbins;
  bool data_reset;

  bool draw_phase;
  bool draw_amplitude;

  bool use_mises;

  bool use_input_as_base;

  double log_sigma;
};

psrover::psrover()
	: Application ("psrover", "program for adding noise to data")
{
  Pulsar::StandardOptions* preprocessor = new Pulsar::StandardOptions;
  add (preprocessor );

  seed = 0;

  nfiles = 1;

  use_input_as_base = data_reset = use_mises = draw_phase = draw_amplitude = got_nbins = got_ascii_file = got_noises = got_fwhms = got_bins = false;

  log_sigma = 0.0;

  i_pol = 0;
  i_chan = 0;

  output_filename = "temp_archive.ar";
  out_path = ".";
  file_count = 0;
  sequential_files = false;
}

void psrover::add_options ( CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  menu.add("");

  menu.add("");

  arg = menu.add( seed, 's',"seed");
  arg->set_help( "set the seed used for generating random numbers");

  arg = menu.add( use_mises, "vM");
  arg->set_help( "use van Mises instead of Gaussian components");

  arg = menu.add( this, &psrover::set_noise,'r',"noise amplitudes");
  arg->set_help( "comma-separated list of noise amplitudes to be added");

  arg = menu.add( draw_amplitude, "fr");
  arg->set_help( "draw normally distributed component amplitude");
  arg->set_long_help ( "the component amplitude specified with -r will be the standard deviation");

  arg = menu.add( log_sigma, "lr", "log_sigma");
  arg->set_help( "draw log-normally distributed component amplitude");
  arg->set_long_help ( "the component amplitude specified with -r will be the mean");

  arg = menu.add( this, &psrover::set_fwhms,'f',"bins");
  arg->set_help( "comma-separated list of component widths");
  arg->set_long_help( "This option will require also to set the centre bins\n"
		  "Negative value means that the corresponding noise amplitude will be the value of delta function\n"
		  "at the corresponding bin, or Gaussian noise, if the corresponding bin is negative as well\n");

  arg = menu.add( this, &psrover::set_bins,'b',"bins");
  arg->set_help ("comma-separated list of component centre bins");
  arg->set_long_help ("Negative value means that the corresponding noise amplitude is the standard deviation of random noise");

  arg = menu.add( draw_phase, "fb");
  arg->set_help( "draw normally distributed component phases (centre bins)");
  arg->set_long_help ( "the component width specified with -f will be the standard deviation");

  menu.add("");
  menu.add("Input / output options");

  arg = menu.add( this, &psrover::set_ascii_file,'a',"ASCII file");
  arg->set_help ("ASCII file with desired underlying profile");
  arg->set_long_help ("if given, it will overwrite the data from archive");

  arg = menu.add( use_input_as_base, 'i', "Use the input archive as a basis for the new profile");
  arg->set_help( "psrover will add the request components on top of the input archive. This is incompatible with -a");

  arg = menu.add( i_pol, "pol", "Choose polarisation to be modified");
  
  arg = menu.add( i_chan, "chan", "Choose frequency channel to be modified");

  arg = menu.add( this, &psrover::set_nbins,'n',"number of bins");
  arg->set_help( "the number of bins in the requested profile");
  arg->set_long_help("This will be overridden by the ASCII file or archive, if any given");

  arg = menu.add( output_filename,'o',"output file");
  arg->set_help( "name of the output, defaults to temp_archive.ar");

  arg = menu.add( nfiles, 'p', "npulses");
  arg->set_help( "Produce npulses simulated pulses" );
  
  arg = menu.add( sequential_files, 'O' );
  arg->set_help( "Use a sequence of numbers as the output filename" );

  arg = menu.add( out_path, 'u', "path" );
  arg->set_help( "Write files to this location" );

  menu.add("");
  arg = menu.add(save_added_noise_filename, "Snoise", "filename");
  arg->set_help("Save the noise added to each bin in ASCII file");

  arg = menu.add(save_added_gauss_filename, "Sgauss", "filename");
  arg->set_help("Save the component amplitude in ASCII file");

  arg = menu.add(save_profile_ascii, "Sascii", "filename");
  arg->set_help("Save the resulting profile in ASCII file");


  menu.add("");

  menu.add("For example the command:\npsrover -r 100,3,30,65,142 -f 30,-1,250,-1,-1 -b 250,-1,400,800,124 inp.ar -a inp.txt -o out.ar\n"
		  "will take input from the ASCII file inp.txt, containing one column, and top of that it will add:\n"
		  "  - first Gaussian component at bin 250 with FWHM 30 and amplitude 100\n"
		  "  - second Gaussian component at bin 400 with FWHM 250 and amplitude 30\n"
		  "  - first spike at bin 800 with amplitude of 65\n"
		  "  - second spike at bin 124 with amplitude of 142\n"
		  "  - white noise with amplitude of 3\n"
		  " The result will be stored in the file out.ar\n");
  menu.add("");
}

void psrover::set_nbins (int _nbins)
{
  nbin = _nbins;
  got_nbins = true;
}

void psrover::set_noise(string _noise)
{
  for (string sub=stringtok(_noise,","); !sub.empty(); sub=stringtok(_noise,","))
    noise_to_add.push_back(fromstring<float>(sub));
  got_noises = true;
}

void psrover::set_fwhms(string _fwhms)
{
  for (string sub=stringtok(_fwhms,","); !sub.empty(); sub=stringtok(_fwhms,","))
    fwhms.push_back(fromstring<float>(sub));
  got_fwhms = true;
}

void psrover::set_bins(string _bins)
{
  for (string sub=stringtok(_bins,","); !sub.empty(); sub=stringtok(_bins,","))
    bins.push_back(fromstring<float>(sub));
  got_bins = true;
}

void psrover::set_ascii_file (string _file)
{
  if (verbose)
    cerr << "psrover::set_ascii_file setting to " << _file << endl;
  ascii_filename = _file;
  got_ascii_file = true;
}

void psrover::setup ()
{
  if (got_ascii_file) {
    if ( use_input_as_base )
    {
      cerr << "psrover::setup illegal combination of options. Use none or one of -a and -i" << endl;
      exit(-1);
    }
    if (verbose)
      cerr << "psrover::setup reading the ASCII file " << endl;
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
    nbin --;
  } //got_ascii_file
  else {
    if (verbose)
      cerr << "psrover::setup defaulting to an empty profile" << endl;
    if (!got_nbins) {
      if (verbose)
       cerr << "psrover::setup no nbins provided, defaulting to 1024" << endl;
      nbin = 1024;
    }
    for (unsigned i = 0 ; i < nbin ; i++) {
      profile_values.push_back(0.0);
    }
  } // didn't get ascii file

  //set the seed
  if (seed == 0)
  {
    if (verbose)
      cerr << "seed from time " << endl;
    seconds = time(NULL);
    gasdev = BoxMuller(seconds);
  }
  else
  {
    if (verbose)
      cerr << "seed from command line " << seed << endl;
    gasdev = BoxMuller(seed);
  }

  //open file for noise
  if (!save_added_noise_filename.empty())
  {
    if (verbose)
      cerr << "psrover::setup opening output file for noise: " << save_added_noise_filename << endl;
    outfile1.open(save_added_noise_filename.c_str());
  }

  //open file for first Gaussian
  if (!save_added_gauss_filename.empty())
  {
    if (verbose)
      cerr << "psrover::setup opening output file for the Gaussian / von Misses amplitudes: " << save_added_gauss_filename << endl;
    outfile2.open(save_added_gauss_filename.c_str(), std::ios::app);
  }
  //open file for the ascii profile
  if (!save_profile_ascii.empty())
  {
    if (verbose)
      cerr << "psrover::setup opening output file for the created profile: " << save_profile_ascii << endl;
    outfile3.open(save_profile_ascii.c_str());
  }
}

void psrover::process (Pulsar::Archive* archive)
{
  // default to adding white noise
  if ( !noise_to_add.empty() && fwhms.empty() && bins.empty() )
  {
    cerr << "No FWHM or bin provided - defaulting to white noise" << endl;
    for ( unsigned i_amp = 0; i_amp < noise_to_add.size (); i_amp++ )
    {
      fwhms.push_back( -1 );
      bins.push_back( -1 );
    }
  }

  nbin = archive-> get_nbin();

  if (verbose)
    cerr << "psrover::process getting the amps from "
	 << archive->get_filename() << endl;

  sequential_files = (nfiles > 1);

  for ( unsigned i_file = 0; i_file < nfiles; i_file++ )
  {
    if (use_mises)
      // start with a fresh model for each output pulse (i_file)
      model = new Pulsar::ComponentModel;

    Reference::To<Archive> clone = archive->clone();

    over (clone);
  }
}

void psrover::over (Archive* archive)
{
  if ( use_input_as_base )
  {
    nbin = archive->get_nbin();
    if (profile_values.size() < nbin)
    {
      for (unsigned i = profile_values.size() ; i < nbin ; i++) {
        profile_values.push_back(0.0);
      }
    }
  }
  float* data = 0;
  float* data_vM = new float[nbin];

  for ( unsigned i_sub = 0; i_sub < archive->get_nsubint() ;i_sub ++ )
  {
    data = archive->get_Integration(i_sub)->get_Profile(i_pol, i_chan)->get_amps();

    if ( use_input_as_base )
    {
      if (verbose)
	cerr << "psrover::process using the input archive as a basis for the new profile" << endl;

      initialise_basis( data, nbin );
      data_reset = true;
    }

    if (!noise_to_add.empty()) {
      for (unsigned jcomp = 0; jcomp < fwhms.size(); jcomp++ ) {
	if (fwhms[jcomp] >= 0 && bins[jcomp] >= 0)
	{
	  int use_bins = bins[jcomp];
	  if (draw_phase)
	  {
	    use_bins += fwhms[jcomp] * gasdev();
	  }
	  if (draw_amplitude)
	  {
	    tmp_rand = gasdev();
	    amplitude = tmp_rand * noise_to_add[jcomp];
	  }
	  else if (log_sigma)
	  {
	    tmp_rand = gasdev();
	    amplitude = noise_to_add[jcomp] * exp(log_sigma * (tmp_rand - log_sigma));
	  }
	  else
	    amplitude = noise_to_add[jcomp];
	  if (!save_added_gauss_filename.empty())
	    outfile2 << amplitude << endl;
	  if (use_mises) {
	    if (verbose)
	    { 
	      cerr << "Adding a von Mises component:" << endl;
	      cerr << "maximum: " << noise_to_add[jcomp] << " peak at the bin: " << use_bins << " FWHM: " << fwhms[jcomp] << endl;
	    }
	    model->add_component(use_bins/nbin, 1.0/fwhms[jcomp]/fwhms[jcomp] * 2.35482 * 2.35482 * nbin * nbin / 4 / M_PI / M_PI, amplitude, "");
	  }
	  else 
	  {
	    if (verbose)
	    {
	      cerr << "Adding a Gaussian:" << endl;
	      cerr << "maximum: " << amplitude << " peak at the bin: " << use_bins << " FWHM: " << fwhms[jcomp] << endl;
	    }
	    for (unsigned ibin = 0; ibin < nbin; ++ibin) {
	      if (!data_reset)
	      {
		if  (verbose && ibin == 0)
		  cerr << "psrover::process Resetting data when adding Gaussian" << endl;
		*data = 0.0;
		if (ibin == nbin-1)
		  data_reset = true;
	      }
	      // resolve the Gaussian with sub_bin resolution:
	      for (unsigned ksubbin = 0; ksubbin < sub_bin; ksubbin++ ) {
		current_bin = (float)((signed)ibin) + 1.0 / sub_bin * (float)ksubbin;
		*data += amplitude / sub_bin * exp( -std::pow((double)current_bin - use_bins,2) / 2 / std::pow(fwhms[jcomp] / 2.35482, 2)) ;
		//wrap the Gaussian around
		*data += amplitude / sub_bin * exp( -std::pow((double)(current_bin - 1024)-use_bins,2) / 2 / std::pow(fwhms[jcomp] / 2.35482, 2)) ;
	      }
	      ++data;
	    }
	  }
	  if (use_mises)
	  {
	    if ( !data_reset ) {
	      model->evaluate(data, archive->get_nbin());
	      data_reset = true;
	    }
	    else {
	      model->evaluate(data_vM, archive->get_nbin());
	      for (unsigned ibin = 0; ibin < nbin; ++ibin)
		data[ibin] += data_vM[ibin];
	    }
	  }
	  else
	  {
	    data -= nbin;
	  } 
	}
	else if (fwhms[jcomp] < 0 && bins[jcomp] < 0)
	{
	  for (unsigned ibin = 0; ibin < nbin; ++ibin)
	  {
	    if (!data_reset && !use_mises)
	    {
	      if (verbose && ibin==0)
		cerr << "psrover::process Resetting data when adding noise" << endl;
	      *data = 0.0;
	      if (ibin == nbin-1)
		data_reset = true;
	    }
	    if (verbose && ibin == 0)
	      cerr << "Adding " << noise_to_add[jcomp] << " noise to each bin" << endl;
	    tmp_rand = gasdev();
	    *data += tmp_rand * (float)noise_to_add[jcomp];
	    if (!save_added_noise_filename.empty())
	      outfile1 << ibin << " " << tmp_rand * (float)noise_to_add[jcomp] << endl;
	    ++data;
	  }
	  data -= nbin;
	}
	else if (fwhms[jcomp] < 0 && bins[jcomp] >= 0)
	{
	  for (unsigned ibin = 0; ibin < nbin; ++ibin)
	  {
	    if (!data_reset)
	    {
	      if (verbose && ibin == 0)
		cerr << "psrover::process Resetting data when adding delta function" << endl;
	      *data = 0.0;
	      if (ibin == nbin - 1)
		data_reset = true;
	    }
	    if (ibin == (unsigned)bins[jcomp])
	    {
	      if (verbose)
		cerr << " Adding " << noise_to_add[jcomp] << " to " << ibin << "-th bin" << endl;
	      *data += noise_to_add[jcomp];
	    }
	    ++data;
	  }
	  data -= nbin;
	}
      }
    }
    if (got_ascii_file)
    {
      if (verbose)
	cerr << "psrover::process adding the values from the ASCII file to the profile" << endl;
      for (unsigned ibin = 0; ibin < nbin; ++ibin)
      {
	if (!data_reset && !use_mises)
	{
	  if (verbose && ibin == 0)
	    cerr << "psrover::process Resetting data when adding ASCII values" << endl;
	  *data = 0.0;
	  if (ibin == nbin - 1)
	    data_reset = true;
	}
	*data += profile_values[ibin];
	++data;
      }
      data -= nbin;
    }

    if (!save_profile_ascii.empty())
    {
      if (verbose)
	cerr << "psrover::process printing profile to an ASCII file" << endl;
      float *amps = archive->get_Profile(i_sub, i_pol, i_chan)->get_amps();
      for (unsigned i = 0 ; i < nbin; i++ )
      {
	outfile3 <<  amps[i] << endl;
      }
    }
    data_reset = false;
  } // end of loop through the sub-integrations
  if (verbose)
    cerr << "Unloading " << endl;
  if ( !sequential_files )
    archive->unload(out_path+"/"+output_filename);
  else
  {
    ss << out_path << "/" << setfill('0') << setw(12) << file_count << ".ar";
    archive->unload( ss.str() );
    ss.str("");
  }
  file_count++;
}


void psrover::initialise_basis( float* data, unsigned nbin )
{
  if ( verbose )
    cerr << "psrover::initialise_basis entered" << endl;
  for (unsigned i = 0; i < nbin; i++ ) {
    profile_values.at(i) = data[i];
  }
}

void psrover::finalize()
{
  if (!save_added_noise_filename.empty())
  {
    if (verbose)
      cerr << "psrover::finalize closing " << save_added_noise_filename << endl;
    outfile1.close();
    outfile2.close();
  }
}

int main (int argc, char** argv) 
{
  psrover program;
  return program.main (argc, argv);
}
