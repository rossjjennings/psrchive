/***************************************************************************
*
*   Copyright (C) 2011 by Stefan Oslowski
*   Licensed under the Academic Free License version 2.1
*
***************************************************************************/

// TODO
// padding with a running average rather than zeros
// S2DFS
using namespace std;

// warn user when multiple paddings performed
#define MAX_PAD 10

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Predictor.h"
#include "Pulsar/Integration.h"
#include "FTransform.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>

class drifting_subpulses : public Pulsar::Application
{
public:

  //! Default constructor
  drifting_subpulses ();

  //! Process the given archive
  void process (Pulsar::Archive*);
  void setup ();
  void finalize ();

  void perform_FFT ();

protected:

  void add_options (CommandLine::Menu&);

  // !bookkeeping and setup variables
  long int fft_size;
  long int current_count;
  long int fft_count;
  bool skip_DC;
  bool pad_missing;
  bool twoD;
  bool randomise;
  bool randomise_coherently_in_time;

  // !Value used for padding data
  float pad_value;

  // !input mean profile filename
  string mean_file;
  // !input mean profile amps
  float *mean_amps;
  // !peak amplitude of the mean profile
  float mean_max;
  // !perform mean subtraction?
  bool subtract_mean;
  // !normalize by peak of the mean?
  bool normalize_by_mean;

  // !output stream
  ofstream outfile;
  // !output postfix
  string postfix;
  // !output directory
  string directory;

  // !variables holding data and resulting spectra
  vector< vector< float > > vec_for_fft;
  vector< float > scrambled_data;
  vector<float*> vec_fft;
  vector<float*> LRFS_final;
  vector<float*> TDFS_final;

  // !variables related to the phase range 
  void choose_range ( string );
  vector<float> phase_range;
  int min_bin, max_bin;
  int nbin;

  // !check for continuity of data
  int64_t previous, current;
  double fcurrent;
};


drifting_subpulses::drifting_subpulses ()
  : Application ("drifting_subpulses", "drifting_subpulses psrchive program")
{
  add( new Pulsar::StandardOptions );
  min_bin = max_bin = -1;

  mean_amps = 0;
  mean_max = 0.0;

  subtract_mean = true;
  normalize_by_mean = true;

  fft_size = -1;
  skip_DC = true;
  pad_missing = true;
  twoD = true;
  current_count = 0;
  fft_count = 0;
  randomise = false;
  randomise_coherently_in_time = false;

  
  previous = current = 0;
  fcurrent = 0;

  pad_value = 0.0;
}

// add command line options
void drifting_subpulses::add_options ( CommandLine::Menu& menu )
{
  CommandLine::Argument* arg;

  menu.add( "" );
  menu.add( "Output options" );
  arg = menu.add( postfix, 'o', "postfix" );
  arg->set_help( "Choose the postfix for output files" );

  arg = menu.add( directory, 'u', "path" );
  arg->set_help( "Write files to this location" );

  menu.add( "" );
  menu.add( "Algorithm setup" );
  arg = menu.add( fft_size, 'f', "FFTsize" );
  arg->set_help( "Choose the size of FFT" );

  arg = menu.add ( skip_DC, "DC" );
  arg->set_help( "Don't ignore the DC term from the first FFT" );
  
  arg = menu.add( twoD, "1D" );
  arg->set_help( "Perform 1D FFT only" );

  arg = menu.add ( this, &drifting_subpulses::choose_range, "pr", "phase_range" );
  arg->set_help( "Choose the phase range for the FFT" );

  arg = menu.add( mean_file, 's', "stdfile" );
  arg->set_help( "Location of the mean profile" );

  arg = menu.add( subtract_mean, 'S' );
  arg->set_help( "Do not subtract the mean" );

  arg = menu.add( normalize_by_mean, 'N' );
  arg->set_help( "Do not normalize by peak of the mean" );

  arg = menu.add( pad_missing, 'p' );
  arg->set_help( "Don't pad the missing integrations with zeros" );

  arg = menu.add( pad_value, 'P', "value" );
  arg->set_help( "Pad the data with the chosen value (default 0.0)" );
  arg->set_long_help( "Powers of 2 are highly recommended to ensure detection of padded data" );

  arg = menu.add( randomise, 'R' );
  arg->set_help( "Randomise the order of data" );

  arg = menu.add( randomise_coherently_in_time, "RC" );
  arg->set_help( "Shuffle the subintegrations once, for the whole phase range simultaneously" );
}


// main function
void drifting_subpulses::process (Pulsar::Archive* archive)
{
  unsigned nsub = archive->get_nsubint();
  nbin = archive->get_nbin();
  archive->fscrunch ();
  archive->pscrunch ();
  Reference::To<const Pulsar::Predictor> model = archive->get_model();
  archive->remove_baseline();

  // on the first pass, initialize the phase range
  if ( max_bin == -1 )
  {
    min_bin = (int)(phase_range.at(0) * (float)nbin);
    max_bin = (int)(phase_range.at(1) * (float)nbin);
    // ensure number of bins is even ( for the performance of FFT )
    if ( twoD && ( max_bin - min_bin + 1 ) % 2 != 0 )
    {
      if ( max_bin < nbin )
	max_bin ++;
      else if ( min_bin > 0 ) 
	min_bin --;
      else
	max_bin --;
    }

    if ( fft_size == -1 )
      fft_size = 1024;

    // push back new float arrays to hold the data
    for (int ibin = 0; ibin < max_bin - min_bin + 1; ibin++) {
      vec_for_fft.push_back( vector< float > (fft_size, 0.0 ) );
    }
  }

  // create various arrays
  float *amps = NULL;
  // create string stream for output file naming
  stringstream ss;

  // loop through the subints of input data
  for (unsigned isub=0; isub < nsub; isub++)
  {
    // calculate the current turn of the pulsar, for padding 
    if ( pad_missing )
    {
      Pulsar::Phase phase = model->phase( archive->get_Integration(isub)->get_epoch ());
      fcurrent = floor (phase.fracturns() + 0.5);
      current = phase.intturns() + int64_t(fcurrent);
    }

    // get the amps of current subint
    amps = archive->get_Profile (isub, 0, 0)->get_amps();

    // if padding requested, check if it is necessary for the current subint
    if ( pad_missing && previous > 0 && current - previous - 1 != 0 )
    {
      // check if data is chronologically sorted
      if ( current - previous -1 < 0 )
      {
	cerr << "WARNING, data appears to be not in chronological order:" << endl;
	cerr << "subint " << isub << " of " << archive->get_filename() << " is earlier or simultaneous than the subint " << isub - 1 << endl;
	cerr << "current and previous turn counts: " << current << " " << previous << endl;
	// should I exit?
      } // end of chronology-warning loop
      // if data is chronological, pad the gaps
      else
      {
	// how many pads were inserted?
	unsigned i_pad = 0;
	while ( i_pad < current - previous - 1 )
	{
	  // pad the whole phase range
	  for ( int ibin = min_bin; ibin <= max_bin; ibin++ )
	  {
	    vec_for_fft.at(ibin - min_bin)[current_count] = pad_value;
	  }
	  // bookkeeping
	  i_pad++;
	  current_count++;
	  // if data buffer was filled by padding, perform the FFTs
	  if ( current_count == fft_size )
	    perform_FFT ();
	}

	// warn if multiple subints are missing
	if ( i_pad > MAX_PAD ) {
		cerr << "WARNING: " << i_pad << " subints missings, consider rejecting data in " << archive->get_filename() << endl;
	}
      } // end of padding loop
    } // end of pad check

    // loop through the bins and add data to buffer
    for ( int ibin = min_bin; ibin <= max_bin; ibin++ )
    {
      vec_for_fft.at(ibin - min_bin)[current_count] = amps[ibin];// - ( subtract_mean && !mean_file.empty() ? mean_amps[ibin] : 0 );
      // if the mean was read from a template, normalize by mean max, possibly subtract the mean as well
      if ( !mean_file.empty() )
      {
	if ( subtract_mean )
	{
	  vec_for_fft.at( ibin - min_bin)[current_count] -= mean_amps[ibin];
	}
	if ( normalize_by_mean )
	  vec_for_fft.at( ibin - min_bin)[current_count] /= mean_max;
      }
    }
    // bookkeeping
    current_count ++;

    // update the previous turn, when padding data
    if ( pad_missing )
      previous = current;

    // check if the data is filled and perform the FFTs if yes
    if ( current_count == fft_size )
    {
      perform_FFT ();
    }
  } // end of loop through the subints
}

// initialisation of variables and setup
void drifting_subpulses::setup ()
{
  // read in the mean amps
  if ( !mean_file.empty() )
  {
    Reference::To<Pulsar::Archive> mean_arch = Pulsar::Archive::load( mean_file );
    mean_arch->fscrunch();
    mean_arch->tscrunch();
    mean_arch->pscrunch();
    mean_arch->remove_baseline();
    mean_amps = new float[ mean_arch->get_nbin() ];

    mean_max = mean_arch->get_Profile( 0, 0, 0 )->max();

    memcpy( mean_amps, mean_arch->get_Profile( 0, 0, 0 )->get_amps(), mean_arch->get_nbin() * sizeof(float) );
  }

  // if postfix is not empty, add an underscore
  if ( !postfix.empty() )
  {
    postfix = "_" + postfix;
  }

  // by default the full phase range is analysed
  if ( phase_range.size () == 0 )
  {
    phase_range.push_back ( 0.0 );
    phase_range.push_back ( 1.0 );
  }

  if ( directory.empty() )
    directory = ".";
}

// finalization of spectra, output
void drifting_subpulses::finalize ()
{
  if ( fft_count > 0 )
  {
    outfile.open( (directory+"/log"+postfix+".dat").c_str() );
    outfile << fft_count << endl;
    outfile.close();
    outfile.open( (directory+"/LRFS"+postfix+".dat").c_str() );
    for (int ibin = 0; ibin < max_bin - min_bin + 1 ; ibin++)
    {
      for (unsigned i=0; i< ( fft_size + 2 ) / 2; i++)
      {
	// the frequency is in P0 / P3 units
	outfile << ibin + min_bin << " " << (float)i / ((float)fft_size) << " " << LRFS_final.at(ibin)[i] / fft_count << endl;
      }
      outfile << endl;
    }
    outfile.close();

    if ( twoD )
    {
      outfile.open( (directory+"/2DFS"+postfix+".dat").c_str() );
      // write out negative frequencies first
      for (int ibin = ( max_bin - min_bin + 1 ) / 2 ; ibin < max_bin - min_bin + 1 ; ibin++  )
      {
	for ( unsigned ifft = 0 + (skip_DC ? 1 : 0 ); ifft < ( fft_size + 2 ) / 2; ifft++ )
	{
	  // frequencies are in P0 / P2 (is it?) and P0 / P3 units.
	  outfile << (float)( (float)ibin - ( max_bin - min_bin + 1) + 1 ) / ((float) max_bin - min_bin + 1 ) * (float)nbin  << " " << ifft / ( (float)fft_size ) << " " << TDFS_final.at(ibin)[ifft] / (float)( max_bin - min_bin + 1 ) << endl;
	}
	outfile << endl;
      }
      // write out positive frequencies
      for (int ibin = 1 ; ibin < ( max_bin - min_bin + 1 ) / 2 ; ibin++ )
      {
	for ( unsigned ifft = 0 + (skip_DC ? 1 : 0 ); ifft < ( fft_size + 2 ) / 2; ifft++ )
	{
	  outfile << (float)( ibin ) / ((float) max_bin - min_bin + 1 ) * (float)nbin  << " " << ifft / ( (float)fft_size ) << " " << TDFS_final.at(ibin)[ifft] / (float)( max_bin - min_bin + 1 ) << endl;
	}
	outfile << endl;
      }
      outfile.close();
    }
  }
  else
    cerr << "WARNING not enough data provided, try decreasing requested fft length" << endl;
}

// calculate LRFS and 2DFS for currently available data chunk
void drifting_subpulses::perform_FFT()
{
  float *x_for_FFT = new float[ 2 * (max_bin - min_bin + 1) ];
  float *x_FFT = new float[ 2 * (max_bin - min_bin + 1) ];
  fft_count++;
  // perform FFT, save output, reset current_count
  current_count = 0;

  // normalize and possibly subtract the mean if no template was given
  if ( mean_file.empty() )
  {
    double *total = new double[max_bin - min_bin + 1];
    unsigned not_padded_count = 0;
    double max_total = 0.0;
    // calculate the mean
    for ( int ibin = 0 ; ibin <= max_bin - min_bin; ibin++ )
    {
      for ( unsigned ifft = 0; ifft < fft_size; ifft++) {
	if ( ifft == 0 )
	  total[ ibin ] = 0.0;
	// only use the data to calculate mean if it were not inserted by padding
	if ( !pad_missing || vec_for_fft.at( ibin )[ ifft ] != pad_value )
	{
	  total[ ibin ] += vec_for_fft.at(ibin)[ ifft ];
	  if ( ibin == 0 )
	    not_padded_count ++;
	}
      }
      if ( total[ ibin ] > max_total )
	max_total = total[ ibin ];
    }
    // subtract and normalize the data
    for (unsigned ifft = 0; ifft < fft_size; ifft++)
    {
      for ( int ibin = 0 ; ibin <= max_bin - min_bin; ibin++ )
      {
	// only normalize if these data were not inserted by padding
	if ( !pad_missing || vec_for_fft.at( ibin )[ifft] != pad_value )
	{
	  if ( subtract_mean )
	  {
	    vec_for_fft.at( ibin )[ifft] =
		    (float)((double)vec_for_fft.at( ibin )[ifft] - total[ibin] / (double)not_padded_count );
	  }
	  if ( normalize_by_mean )
	  {
	    vec_for_fft.at( ibin )[ifft] = vec_for_fft.at( ibin )[ifft] / max_total * (double)not_padded_count;
	  }
	}
      }
    }
    delete [] total;
  }// finished normalization and subtraction of the mean based on input data

  // DEBUG
  if ( fft_count == 1 )
  {
    outfile.open( "vec_for_fft.dat" );
    for ( unsigned i = 0; i < fft_size; i++ )
    {
      outfile << vec_for_fft.at( 0 )[i] << " " << vec_for_fft.at( 40 )[i] << " " << vec_for_fft.at( 120 )[i] << " " << vec_for_fft.at( 127 )[i] << " " << vec_for_fft.at( 128 )[i] << " " << vec_for_fft.at( 235 )[i] <<  endl;
    }
    outfile.close();
  }

  // calculate fluctuation spectrum for every bin
  for ( int ibin = 0 ; ibin <= max_bin - min_bin; ibin++ )
  {
    // allocate memory if this wasn't done before
    if ( vec_fft.size() != vec_for_fft.size() )
    {
      vec_fft.push_back( new float[ fft_size + 2 ] );
      LRFS_final.push_back( new float[ (fft_size + 2) / 2 ] );
      if ( twoD )
	TDFS_final.push_back( new float[ ( fft_size + 2 ) / 2 ] ) ;
      for (unsigned i = 0; i < (fft_size + 2) / 2 ; i++) {
	LRFS_final.at(ibin)[i] = 0.0;
	if ( twoD )
	  TDFS_final.at(ibin)[i] = 0.0;
      }
    }
    // TODO randomise the order of data, useful for generating masks and testing
    if ( randomise )
    {
	// copy the data for given bin.
	scrambled_data = vec_for_fft.at(ibin);
	// remove padded data
	// TODO this can be made more efficient by storing the padded i for first bin and then using that information for other bins. 
	// Hm, the ways I considered so far wouldn't be that much more efficient. Plus the main cost is the fft and I/O anyway.
	unsigned deleted_count = 0;
	for ( unsigned i = 0; i < fft_size; i++ )
	{
	  if ( scrambled_data.at( i - deleted_count ) == pad_value )
	  {
	    // DEBUG
	    if ( *(scrambled_data.begin() + i - deleted_count ) != scrambled_data.at(i - deleted_count ) )
	      cerr << "WARNING iterator and at mismatch! " << scrambled_data.at(i) << " vs " << *(scrambled_data.begin() + i) << " " << i << endl;

	    scrambled_data.erase(scrambled_data.begin() + i - deleted_count );
	    deleted_count++;
	  }
	}
	// scramble data
	random_shuffle( scrambled_data.begin(), scrambled_data.end() );
	// copy data back
	unsigned used_count = 0;
	for ( unsigned i = 0; i < fft_size; i++ )
	{
	  if ( vec_for_fft.at(ibin)[i] != pad_value )
	  {
	    vec_for_fft.at(ibin)[i] = scrambled_data[used_count] ;
	    used_count++;
	  }
	}
	if ( used_count != scrambled_data.size() )
	  cerr << "WARNING: used only " << used_count << " of scrambled data when expected " << scrambled_data.size() << endl;
	// DEBUG
	if ( fft_count == 1 )
	{
	  outfile.open( "vec_for_fft_scrambled.dat" );
	  for ( unsigned i = 0; i < fft_size; i++ )
	  {
	    outfile << vec_for_fft.at( 0 )[i] << " " << vec_for_fft.at( 40 )[i] << " " << vec_for_fft.at( 120 )[i] << " " << vec_for_fft.at( 127 )[i] << " " << vec_for_fft.at( 128 )[i] << " " << vec_for_fft.at( 235 )[i] <<  endl;
	  }
	  outfile.close();
	}
    }
    // calculate LRFS
    FTransform::frc1d ( fft_size, vec_fft.at(ibin), &(vec_for_fft.at(ibin)[0]) );

    // add the power in current LRFS to the final spectrum
    for ( unsigned i = 0; i < (fft_size + 2) / 2; i++ )
    {
      LRFS_final.at(ibin)[i] += pow( vec_fft.at(ibin)[i*2]/(float)fft_size, 2 ) + pow( vec_fft.at(ibin)[i*2+1]/(float)fft_size, 2 ) ;
    }
  } // end of a loop through the bins

  // handle 2D fluctuation spectra
  if ( twoD )
  {
    unsigned skip = skip_DC ? 1 : 0;

    // loop through all the ffts from LRFS
    for ( unsigned ifft = 0 + skip; ifft < (fft_size + 2) / 2; ifft++ )
    {
      // copy over data in longitude direction to an auxiliary variable 
      for ( int ibin = 0 ; ibin <= max_bin - min_bin; ibin++ )
      {
	x_for_FFT[2*ibin] = vec_fft.at(ibin)[ifft*2];
	x_for_FFT[2*ibin+1] = vec_fft.at(ibin)[ifft*2+1];
      }
      // calculate ifft'th part of the 2DFS
      FTransform::fcc1d ( max_bin - min_bin + 1, x_FFT, x_for_FFT );

      // add the power in current 2DFS to the final spectrum
      for ( int ibin = 0 ; ibin <= max_bin - min_bin; ibin++ )
      {
	TDFS_final.at(ibin)[ifft] += pow( x_FFT[ibin*2] / ((float)fft_size + 2 ) / 2.0, 2)  + pow( x_FFT[ibin*2+1]/((float)fft_size + 2 ) / 2.0, 2 );
      }
    }
  }
  delete[] x_FFT;
  delete[] x_for_FFT;
}

int main (int argc, char** argv)
{
  drifting_subpulses program;
  return program.main (argc, argv);
}

// choose the phase range
void drifting_subpulses::choose_range ( string _range )
{
  if ( verbose )
    cerr << "psrspa::choose_range parsing " << _range << " as a list of phase ranges" << endl;
  for ( string sub = stringtok ( _range, "," ); !sub.empty(); sub = stringtok ( _range, "," ) )
  {
    phase_range.push_back(fromstring<float>(sub));
  }
  if ( phase_range.size()%2 != 0 )
    throw Error ( InvalidParam, "psrspa::chose_range", "Minimum and maximum needs to be provided" );
}
