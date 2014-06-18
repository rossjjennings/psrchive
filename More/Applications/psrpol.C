/***************************************************************************
 *
 *   Copyright (C) 2014 by Stefan Oslowski
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* pulsar polarisation - an application for polarisation studies that uses Hierarchical Equal Area isoLatitude Pixelization of a sphere
 * (HEALPIX) to represent the distributions of polarisation properties
 */

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/PlotOptions.h"
#include "Pulsar/ProfileWeightFunction.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/ProfileStats.h"
#include "Pulsar/PhaseWeight.h"

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>

#include <cpgplot.h>

#include "strutil.h"
#include "dirutil.h"
#include "Error.h"
#include "Warning.h"

#include "healpix_map.h"
#include "healpix_map_fitsio.h"
#include "fitshandle.h"

using namespace std;
using namespace Pulsar;
static Warning warn;

class psrpol : public Application
{
public:
  //! Default constructor:
  psrpol ();

  //! Process the given archive
  void process ( Archive* );

  //! Final processing
  void finalize ();

  //! Initial setup
  void setup ();

protected:
  void add_options( CommandLine::Menu& );

  //! phase range setup
  void choose_range( string );
  vector<float> phase_range;

  void setup_phase_range( unsigned );
  bool phase_range_built;

  //! Order of healpix maps
  int healpix_order;

  //! Ordering scheme of healpix maps
  string healpix_scheme;

  unsigned nbin;

  //! Stokes parameters:
  float* Q;
  float* U;
  float* V;
  //! Mean Stokes parameters
  vector<vec3> mean_Stokes;  
  //! Total intensity:
  float* I;

  //! Baseline standard
  PhaseWeight* baseline_std;
  string baseline_std_name;

  //! Baseline variances:
  double Q_base_var;
  double U_base_var;
  double V_base_var;

  //! Average polarised baseline rms
  float baseline_rms;

  //! Baseline worker
  ProfileStats stats;

  //! Threshold
  float p_threshold;

  //! Weighting of the data
  bool weight_by_pol_flux;

  //! Healpix workers
  vector< Healpix_Map<double> > healpix_maps;
  vector<unsigned> healpix_maps_bins;

  //! Output files names
  string out_name;
  string bins_out_name;
};

// default constructor
psrpol::psrpol ()
	: Application ( "psrpol", "Pulsar Polarisation")
{
  add ( new StandardOptions );
  healpix_order = 3;
  healpix_scheme = "RING";
  p_threshold = 3.0;
  out_name="healpix.fits";
  phase_range_built = false;
}

void psrpol::setup ()
{
  weight_by_pol_flux = true;
  if ( verbose )
    cerr << "psrpol::setup entered" << endl;
  nbin = 0;

  if ( !baseline_std_name.empty () )
  {
    if ( p_threshold > 0.0 )
    {
    if ( verbose )
      cerr << "psrpol::setup deriving baseline from " << baseline_std_name << endl;
    Reference::To<Archive> baseline_std_arch = Archive::load( baseline_std_name );
    baseline_std_arch->remove_baseline ();
    baseline_std_arch->fscrunch ();
    baseline_std_arch->tscrunch ();
    baseline_std_arch->pscrunch ();
    stats.select_profile( baseline_std_arch->get_Profile( 0, 0, 0 ) );
    }
    else
    {
      warn << "psrpol::setup threshold set to 0.0, not deriving baseline" << endl;
    }
  }

  if ( verbose )
    cerr << "psrpol::setup finished" << endl;
}


//this part of setup requires prior knowledge of nbin
void psrpol::setup_phase_range( unsigned _nbin )
{
  if ( phase_range.size() == 0 )
  {
    if ( verbose )
      cerr <<"psrpol::setup setting default phase_range" << endl;
    phase_range.push_back( 0.0 );
    phase_range.push_back( 1.0 );
  }

  if ( verbose )
    cerr << "psrpol::setup processing phase range and initialising healpix_maps" << endl;
  unsigned low_end, high_end;
  for ( unsigned irange = 0; irange < phase_range.size (); irange++ )
  {
    if ( irange%2 == 0)
    {
      low_end = unsigned( floor ( phase_range[irange] * float(_nbin) + 0.5) );
    }
    else
    {
      high_end = unsigned( floor ( phase_range[irange] * float(_nbin) + 0.5 ) );
      if ( low_end > high_end )
      {
	cerr << low_end << " (" << phase_range[irange-1] << ") " << high_end << " (" << phase_range[irange] << ") "<< endl;
	throw Error ( InvalidParam, "psrpol::setup", "wrong range provided" );
      }

      unsigned current_map_index = 0;
      for (unsigned ibin=low_end; ibin < high_end; ++ibin )
      {
	healpix_maps_bins.push_back( ibin );
	Healpix_Map<double> _map;
	healpix_maps.push_back( _map );
	healpix_maps[current_map_index].Set ( healpix_order, string2HealpixScheme(healpix_scheme) );
	healpix_maps[current_map_index].fill( 0.0 );
	current_map_index++;
      }
    }
  }

  vec3 zero_v3;
  zero_v3.Set( 0.0, 0.0, 0.0 );
  mean_Stokes.resize( healpix_maps_bins.size(), zero_v3 );

  if ( healpix_maps_bins.size() > 999 )
  {
    cerr << "psrpol::setup FITS file don't suppport more than 999 maps per file. psrpol currently doesn't support multiple output files" << endl;
    exit( -1 );
  }
  phase_range_built = true;
}

void psrpol::process ( Archive* archive )
{
  if ( nbin == 0 )
  {
    nbin = archive->get_nbin ();

    Q = new float[nbin];
    U = new float[nbin];
    V = new float[nbin];
  }

  if ( !phase_range_built )
    setup_phase_range ( nbin );
  if ( verbose )
    cerr << "psrpol::process entered" << endl;

  archive->fscrunch ();
  archive->convert_state ( Signal::Stokes );
  archive->remove_baseline ();
  if ( verbose )
    cerr << "psrpol::process perpared " << archive->get_filename () << " archive" << endl;

  Reference::To<PolnProfile> pprofile;
  Reference::To<Profile> profile;

  for ( unsigned isub = 0; isub < archive->get_nsubint (); ++isub )
  {
    if ( verbose )
      cerr << "psrpol::process looping through subints, currently " << isub << endl;
    
    if ( !weight_by_pol_flux )
      I = archive->get_Profile( isub, 0, 0 )->get_amps ();
    profile = archive->get_Profile(isub, 1, 0 );
    Q = profile->get_amps ();
    if ( p_threshold > 0.0 )
    {
      stats.set_profile ( profile );
      Q_base_var = stats.get_baseline_variance().get_value () ;
    }

    profile = archive->get_Profile(isub, 2, 0 );
    U = profile->get_amps ();
    if ( p_threshold > 0.0 )
    {
      stats.set_profile ( profile );
      U_base_var = stats.get_baseline_variance().get_value () ;
    }

    profile = archive->get_Profile(isub, 3, 0 );
    V = profile->get_amps ();
    if ( p_threshold > 0.0 )
    {
      stats.set_profile ( profile );
      V_base_var = stats.get_baseline_variance().get_value () ;
    }
    
    if ( p_threshold <= 0.0 )
      baseline_rms = 0.0;

    baseline_rms = sqrt ( Q_base_var + U_base_var + V_base_var );

    if ( verbose )
      cerr << "psrpol::process set Q,U and V amps" << endl;

    unsigned ibin;
    double weight;
    for ( unsigned imap = 0; imap < healpix_maps_bins.size(); ++imap )
    {
      ibin = healpix_maps_bins[imap];
      const vec3 pol = vec3 ( Q[ibin], U[ibin], V[ibin] );
      if ( !bins_out_name.empty() )
      {
	mean_Stokes.at(imap) += pol;
      }
      weight = weight_by_pol_flux ? sqrt( Q[ibin]*Q[ibin] + U[ibin]*U[ibin] + V[ibin]*V[ibin] ) : I[ibin];
      if ( pol.Length() > p_threshold * baseline_rms )
	healpix_maps[imap][healpix_maps[imap].vec2pix( pol )] += weight;
    }
  } // end of the loop through subints
} // process

void psrpol::finalize ()
{
  if ( verbose )
    cerr << "psrpol::finalize entered" << endl;
  fitshandle fh;
  fh.create( out_name );
  stringstream ss;
  arr<string> colname( healpix_maps_bins.size() );
  for (unsigned imap = 0; imap < healpix_maps_bins.size (); ++imap )
  {
    ss << "bin_" << healpix_maps_bins[imap];
    colname[imap] = ss.str();
    ss.str( "" );
  }

  prepare_Healpix_fitsmap( fh, healpix_maps[0], PLANCK_FLOAT64, colname );

  unsigned file_count=0;
  for (unsigned imap = 0; imap < healpix_maps_bins.size (); ++imap )
  {
    fh.write_column( imap+1, healpix_maps[imap].Map() );
  }

  if ( !bins_out_name.empty() )
  {
    ofstream out_stream;
    out_stream.open( bins_out_name.c_str() );//, ios::out | ios::trunc );
    for ( unsigned ibin = 0; ibin < healpix_maps_bins.size (); ibin++ )
    {
      //mean_Stokes.at(ibin).Normalize() ;
      out_stream << healpix_maps_bins[ibin] << " " << float(healpix_maps_bins[ibin]) / float(nbin) << " " << mean_Stokes.at(ibin).x << " " << mean_Stokes.at(ibin).y << " " << mean_Stokes.at(ibin).z << " " << acos(mean_Stokes.at(ibin).z) << " " << atan(mean_Stokes.at(ibin).y / mean_Stokes.at(ibin).x ) << endl;
    }
    out_stream.close();
  }

  if ( verbose )
    cerr << "psrpol::finalize finished" << endl;
}

void psrpol::add_options ( CommandLine::Menu& menu )
{
  CommandLine::Argument* arg;

  menu.add( "" );
  menu.add( "Basic setup:" );
  arg = menu.add( this, &psrpol::choose_range, "pr","min,max(,min,max...)" );
  arg->set_help( "Specify phase range (in turns) for the construction of healpix maps" );
  arg->set_long_help( "Multiple ranges can be provided, e.g., '0.0,0.1,0.3,0.35' will create two ranges 0.0-0.1 and 0.3-0.35" );

  arg = menu.add( healpix_order, "ho", "order" );
  arg->set_help( "Set order (i.e. the resolution parameter) of healpix maps" );

  arg = menu.add( healpix_scheme, "hs", "scheme" );
  arg->set_help( "Set the ordering scheme for healpix maps" );
  arg->set_long_help( "This has to be set to RING (for quick spectral analysis) or NEST (for quick nearest neighbour analysis)" );

  arg = menu.add ( baseline_std_name, "s", "standard" );
  arg->set_help( "Set the standard file, used to derive baseline." );
  arg->set_long_help( "If not provided the baseline will be estimated for each integration separately which can slow psrpol even by a factor of ~5" );

  arg = menu.add( p_threshold, "t", "threshold" );
  arg->set_help( "Set the threshold for including data, in units of off-pulse polarisation rms" );

  arg = menu.add( weight_by_pol_flux, "I" );
  arg->set_help( "Weight the data by total intensity rather than polarised flux" );

  menu.add ( "" );
  menu.add ( "Output setup:" );

  arg = menu.add( out_name, "o", "output_file" );
  arg->set_help( "Set the name of the FITS file with healpix maps" );
  
  arg = menu.add ( bins_out_name, "b", "bins_output_file" );
  arg->set_help( "Set the name of text file with a list of bins used" );
}

int main ( int argc, char** argv )
{
  psrpol program;
  return program.main (argc, argv);
}

void psrpol::choose_range ( string _range )
{
  if ( verbose )
    cerr << "psrpol::choose_range parsing " << _range << " as a list of phase ranges" << endl;
  for ( string sub = stringtok ( _range, "," ); !sub.empty(); sub = stringtok ( _range, "," ) )
  {
    phase_range.push_back(fromstring<float>(sub));
  }
  if ( phase_range.size()%2 != 0 )
    throw Error ( InvalidParam, "psrpol::choose_range", "Minimum and maximum needs to be provided" );
}

