//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Applications/pdv.C,v $
   $Revision: 1.10 $
   $Date: 2007/12/10 04:16:48 $
   $Author: nopeer $ */


#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Interpreter.h"
#include "Pulsar/PolnProfile.h"
#include <Pulsar/ProcHistory.h>
#include <table_stream.h>
#include <algorithm>
#include <functional>

#include "strutil.h"
#include "dirutil.h"
#include <tostring.h>

#include <iostream>
#include <vector>


#define HELP_KEY             'h'
#define IBIN_KEY             'b'
#define ICHAN_KEY            'n'
#define ISUB_KEY             'i'
#define PHASE_KEY            'r'
#define FSCRUNCH_KEY         'F'
#define TSCRUNCH_KEY         'T'
#define PSCRUNCH_KEY         'P'
#define CENTRE_KEY           'C'
#define BSCRUNCH_KEY         'B'
#define STOKES_FRACPOL_KEY   'x'
#define STOKES_FRACLIN_KEY   'y'
#define STOKES_FRACCIR_KEY   'z'
#define STOKES_POSANG_KEY    'Z'
#define CALIBRATOR_KEY       'c'
#define PULSE_WIDTHS_KEY     'f'
#define BASELINE_KEY         'R'
#define TEXT_KEY             't'
#define TEXT_HEADERS_KEY     'A'
#define PER_SUBINT_KEY       'S'
#define HISTORY_KEY          'H'


using namespace std;
using namespace Pulsar;


bool cmd_text = false;
bool cmd_flux = false;
bool cmd_subints = false;
bool cmd_history = false;
bool per_channel_headers = false;
bool cal_parameters = false;
bool keep_baseline = false;

bool show_pol_frac = false;
bool show_lin_frac = false;
bool show_circ_frac = false;
bool show_pa = false;

int ichan = -1;
int ibin = -1;
int isub = -1;
float phase = 0.0;

vector<string> jobs;

// default duty cycle
float dc = 0.15;




void Usage( void )
{
  cout <<
  "A program for extracting archive data in text form \n"
  "Usage: \n"
  "     pdv [-f dc] [-H params] [-t] [-c] filenames \n"
  "Where: \n"
  "   -" << IBIN_KEY <<           " ibin     select a single phase bin, from 0 to nbin-1 \n"
  "   -" << ICHAN_KEY <<          " ichan    select a single frequency channel, from 0 to nchan-1 \n"
  "   -" << ISUB_KEY <<           " isub     select a single integration, from 0 to nsubint-1 \n"
  "   -" << PHASE_KEY <<          " phase    rotate the profiles by phase before printing \n"
  "   -" << FSCRUNCH_KEY <<       "          Fscrunch first \n"
  "   -" << TSCRUNCH_KEY <<       "          Tscrunch first \n"
  "   -" << PSCRUNCH_KEY <<       "          Pscrunch first \n"
  "   -" << CENTRE_KEY <<         "          Centre first \n"
  "   -" << BSCRUNCH_KEY <<       " factor   Bscrunch by this factor first \n"
  "   -" << STOKES_FRACPOL_KEY << "          Convert to Stokes and also print fraction polarisation \n"
  "   -" << STOKES_FRACLIN_KEY << "          Convert to Stokes and also print fraction linear \n"
  "   -" << STOKES_FRACCIR_KEY << "          Convert to Stokes and also print fraction circular \n"
  "   -" << STOKES_POSANG_KEY <<  "          Convert to Stokes and also print position angle \n"
  "   -" << CALIBRATOR_KEY <<     "          Print out calibrator (square wave) parameters \n"
  "   -" << PULSE_WIDTHS_KEY <<   " dcyc     Show pulse widths and mean flux density (mJy) \n"
  "                                          with baseline width dcyc \n"
  "   -" << BASELINE_KEY <<       "          Do not remove baseline \n"
  "   -" << TEXT_KEY <<           "          Print out profiles as ASCII text \n"
  "   -" << TEXT_HEADERS_KEY <<   "          Print out profiles as ASCII text (with per channel headers) \n"
  // "   -" << PER_SUBINT_KEY <<     "          Print out per subint data \n"
  "   -" << HISTORY_KEY <<        "          Print out the history table for the archive \n"
  << endl;
}



void Header( Reference::To< Pulsar::Archive > archive )
{
  cout << "File: " << archive->get_filename()
  << " Src: " << archive->get_source()
  << " Nsub: " << archive->get_nsubint()
  << " Nch: " << archive->get_nchan()
  << " Npol: " << archive->get_npol()
  << " Nbin: " << archive->get_nbin() << endl;
}

void IntegrationHeader( Reference::To< Pulsar::Integration > intg )
{
  cout << "MJD(mid): " << intg->get_epoch().printdays(12);
  tostring_precision = 3;
  cout << " Tsub: " << tostring<double>( intg->get_duration() );
}

void OutputDataAsText( Reference::To< Pulsar::Archive > archive )
{
  unsigned nsub = archive->get_nsubint();
  unsigned npol = archive->get_npol();
  unsigned nchn = archive->get_nchan();
  unsigned nbin = archive->get_nbin();

  int fchan = 0, lchan = nchn - 1;
  if( ichan != -1 && ichan <= lchan && ichan >= fchan)
  {
    fchan = ichan;
    lchan = ichan;
  }

  int fbin = 0, lbin = archive->get_nbin() - 1;
  if( ibin <= lbin && ibin >= fbin )
  {
    fbin = ibin;
    lbin = ibin;
  }

  int fsub = 0, lsub = archive->get_nsubint() - 1;
  if( isub <= lsub && isub >= fsub )
  {
    fsub = isub;
    lsub = isub;
  }

  tostring_places = true;

  try
  {
    if( nsub > 0 )
    {
      Header( archive );

      for (unsigned s = fsub; s <= lsub; s++)
      {
        Integration* intg = archive->get_Integration(s);
        for (unsigned c = fchan; c <= lchan; c++)
        {
          vector< Estimate<double> > PAs;
          if( show_pa )
          {
            Reference::To<Pulsar::PolnProfile> profile;
            profile = intg->new_PolnProfile(c);
            profile->get_orientation (PAs, 3.0);
          }

          if( per_channel_headers )
          {
            IntegrationHeader( intg );
            cout << " ChFreq: " << tostring<double>( intg->get_centre_frequency( c ) );
            cout << " ChBW: " << intg->get_bandwidth() / nchn;
            cout << endl;
          }
          for (unsigned b = fbin; b <= lbin; b++)
          {
            cout << s << " " << c << " " << b;
            for(unsigned ipol=0; ipol<npol; ipol++)
            {
              Profile *p = intg->get_Profile( ipol, c );
              cout << " " << p->get_amps()[b];
            }
            if( show_pol_frac || show_lin_frac || show_circ_frac || show_pa )
            {
              float stokesI = intg->get_Profile(0,c)->get_amps()[b];
              float stokesQ = intg->get_Profile(1,c)->get_amps()[b];
              float stokesU = intg->get_Profile(2,c)->get_amps()[b];
              float stokesV = intg->get_Profile(3,c)->get_amps()[b];

              float frac_lin  = sqrt(stokesQ*stokesQ + stokesU*stokesU)/stokesI;
              float frac_circ = fabs(stokesV)/stokesI;
              float frac_pol  = sqrt(stokesQ*stokesQ + stokesU*stokesU + stokesV*stokesV)/stokesI;

              if( show_pol_frac )  cout << " " << frac_pol;
              if( show_lin_frac )  cout << " " << frac_lin;
              if( show_circ_frac ) cout << " " << frac_circ;
              if( show_pa )        cout << " " << PAs[b].get_value();
            }
            cout << endl;
          }
        }
      }
    }
  }
  catch ( Error e )
  {
    cerr << e << endl;
  }
}



float flux (const Profile* profile, float dc, float min_phs)
{
  if( min_phs < 0.0 )
    min_phs = profile->find_min_phase(dc);

  double min_avg = profile->mean(min_phs, dc);

  if( Profile::verbose )
    fprintf(stderr,"Pulsar::flux() got dc=%f min_phs=%f max_phs=%f min_avg=%f\n",
            dc,min_phs,profile->find_max_phase(dc),min_avg);

  // Find the total flux in the profile
  double flux = profile->sum();

  // Subtract the total flux due to the baseline
  flux -= (min_avg * profile->get_nbin());
  flux = flux/profile->get_nbin();

  return flux;
}



float width (const Profile* profile, float& error, float pc, float dc)
{

  float min_phs = profile->find_min_phase(dc);
  int   min_bin = int(min_phs * float(profile->get_nbin()));

  double min_mean = 0.0;
  double min_var  = 0.0;
  double var_var  = 0.0;

  int start = int(float(profile->get_nbin()) * (min_phs - dc/2.0));
  int end   = int(float(profile->get_nbin()) * (min_phs + dc/2.0));

  profile->stats(&min_mean, &min_var, &var_var, start, end);

  float stdev = sqrt(min_var);

  float level = profile->max() * pc/100.0 - stdev;

  std::vector<float> results;

  for (unsigned tries = 0; tries < 3; tries++)
  {

    if (level < 5.0*min_var)
    {
      return 0.0;
    }
    float c = 0.0;
    float m = 0.0;
    float lo_edge = 0.0;
    float hi_edge = 0.0;

    int bc = min_bin;

    while (1)
    {
      if (profile->get_amps()[bc] > level)
      {
        // Linerly interpolate between both sides of the edge
        int x1 = bc;
        int x2 = bc+1;

        float y1 = profile->get_amps()[x1];
        float y2 = profile->get_amps()[x2];

        m = (y2 - y1) / (float(x2) - float(x1));
        c = y1 - m*x1;

        hi_edge = (level - c)/m;

        break;
      }
      bc--;
      if (bc < 0)
        bc += profile->get_nbin();
      if (bc == min_bin)
        break;
    }

    bc = min_bin;

    while (1)
    {
      if (profile->get_amps()[bc] > level)
      {
        // Linerly interpolate between both sides of the edge
        int x1 = bc;
        int x2 = bc-1;

        float y1 = profile->get_amps()[x1];
        float y2 = profile->get_amps()[x2];

        m = (y2 - y1) / (float(x2) - float(x1));
        c = y1 - m*x1;

        lo_edge = (level-c)/m;

        break;
      }
      bc++;
      if (bc > int(profile->get_nbin()-1))
        bc -= profile->get_nbin();
      if (bc == min_bin)
        break;
    }

    float bin_dist = hi_edge - lo_edge;
    if(bin_dist < 0)bin_dist += float(profile->get_nbin());

    results.push_back(bin_dist / float(profile->get_nbin()));
    level += stdev;

  }

  error = fabs(results.front()-results.back())/2.0;
  return results[1];
}


void CalParameters( Reference::To< Archive > archive )
{
  if (archive->get_npol() == 4)
    archive->convert_state (Signal::Stokes);

  int fchan = 0, lchan = archive->get_nchan() - 1;
  if( ichan != -1 && ichan <= lchan && ichan >= fchan)
  {
    fchan = ichan;
    lchan = ichan;
  }

  int fsub = 0, lsub = archive->get_nsubint() - 1;
  if( isub <= lsub && isub >= fsub )
  {
    fsub = isub;
    lsub = isub;
  }

  Header( archive );

  vector< vector< Estimate<double> > > hi;
  vector< vector< Estimate<double> > > lo;

  for (unsigned s = fsub; s <= lsub; s++)
  {
    Integration* intg = archive->get_Integration (s);

    intg->cal_levels(hi,lo);
    IntegrationHeader( intg );
    cout << endl;

    cout << "isub ichan freq hi_err";
    for (unsigned ipol = 0; ipol < archive->get_npol(); ipol++)
      cout << " hi_pol" << ipol;
    cout << " lo_err";
    for (unsigned ipol = 0; ipol < archive->get_npol(); ipol++)
      cout << " lo_pol" << ipol;
    cout << endl;

    for (unsigned c = fchan; c <= lchan; c++)
    {
      cout << s << " "
      << c << " " << intg->get_centre_frequency( c ) << " "
      << hi[0][c].get_error();

      for (unsigned ipol = 0; ipol < archive->get_npol(); ipol++)
        cout << " " << hi[ipol][c].get_value();

      cout << " " << lo[0][c].get_error();

      for (unsigned ipol = 0; ipol < archive->get_npol(); ipol++)
        cout << " " << lo[ipol][c].get_value();

      cout << endl;

    }

  }

}


void Flux( Reference::To< Archive > archive )
{
  archive->centre();

  if (archive->get_npol() == 4)
    archive->convert_state (Signal::Stokes);

  //   if( !keep_baseline )
  //     archive->remove_baseline ();

  cout << "File\t\t\tSub\tChan\tPol\tFlux\tUnit\t10\% Width\t50\% Width"
  << endl;

  int fchan = 0, lchan = archive->get_nchan() - 1;
  if( ichan <= lchan && ichan >= fchan)
  {
    fchan = ichan;
    lchan = ichan;
  }

  int fsub = 0, lsub = archive->get_nsubint() - 1;
  if( isub <= lsub && isub >= fsub )
  {
    fsub = isub;
    lsub = isub;
  }

  float junk;

  for (unsigned s = fsub; s <= lsub; s++)
  {
    for (unsigned c = fchan; c <= lchan; c++)
    {
      for (unsigned k = 0; k < archive->get_npol(); k++)
      {
        cout << archive->get_filename() << "\t";
        cout << s << "\t" << c << "\t" << k << "\t";
        cout.setf(ios::showpoint);
        cout << flux(archive->get_Profile(s,k,c),dc, -1);
        if (archive->get_scale() == Signal::Jansky)
          cout << "\t" << "mJy";
        else
          cout << "\t" << "Arb";
        cout << "\t" << width(archive->get_Profile(s,k,c),junk, 10,dc);
        if (width(archive->get_Profile(s,k,c),junk, 10,dc) == 0)
          cout << "\t";
        cout << "\t" << width(archive->get_Profile(s,k,c),junk, 50,dc)
        << endl;
      }
    }
  }
}



void DisplaySubints( vector<string> filenames )
{
  cerr << "per subint data" << endl;
}




void DisplayHistory( vector<string> filenames, vector<string> params )
{
  if( params.size() == 0 || filenames.size() == 0 )
  {
    cerr << "Usage: pav -H param1,param2 filenames (params are date_pro,proc_cmd etc)" << endl;
  }
  else
  {
    vector<string>::iterator fit;
    for( fit = filenames.begin(); fit != filenames.end(); fit ++ )
    {
      table_stream ts(&cout);

      ts << "Filename";
      vector<string>::iterator pit;
      for( pit = params.begin(); pit != params.end(); pit ++ )
      {
        ts << (*pit);
      }
      ts << endl;

      Reference::To<Archive> archive = Archive::load( (*fit) );
      if( !archive )
      {
        ts << (*fit) << "Failed To Load" << endl;
      }
      else
      {
        Reference::To<ProcHistory> history = archive->get<ProcHistory>();

        if( !history )
        {
          ts << (*fit) << "No History Table" << endl;
        }
        else
        {
          vector<ProcHistory::row>::iterator rit;
          for( rit = history->rows.begin(); rit != history->rows.end(); rit ++ )
          {
            ts << (*fit);

            vector<string>::iterator pit;
            for( pit = params.begin(); pit != params.end(); pit ++ )
            {
              if( (*pit) == "date_pro" )
                ts << (*rit).date_pro;
              else if( (*pit) == "proc_cmd" )
                ts << (*rit).proc_cmd;
	      else if( (*pit) == "scale" )
		ts << tostring<Signal::Scale>( (*rit).scale );
	      else if( (*pit) == "pol_type" )
		ts << (*rit).pol_type;
	      else if( (*pit) == "npol" )
		ts << tostring<int>( (*rit).npol );
	      else if( (*pit) == "nbin" )
		ts << tostring<int>( (*rit).nbin );
	      else if( (*pit) == "nsub" )
		ts << tostring<int>( (*rit).nsub );
	      else if( (*pit) == "nbin_prd" )
		ts << tostring<int>( (*rit).nbin_prd );
	      else if( (*pit) == "tbin" )
		ts << tostring<double>( (*rit).tbin );
	      else if( (*pit) == "ctr_freq" )
		ts << tostring<double>( (*rit).ctr_freq );
	      else if( (*pit) == "nchan" )
		ts << tostring<int>( (*rit).nchan );
	      else if( (*pit) == "chan_bw" )
		ts << tostring<double>( (*rit).chanbw );
	      else if( (*pit) == "par_corr" )
		ts << tostring<int>( (*rit).par_corr );
	      else if( (*pit) == "fa_corr" )
		ts << tostring<int>( (*rit).fa_corr );
	      else if( (*pit) == "rm_corr" )
		ts << tostring<int>( (*rit).rm_corr );
	      else if( (*pit) == "dedisp" )
		ts << tostring<int>( (*rit).dedisp );
	      else if( (*pit) == "dds_mthd" )
		ts << (*rit).dds_mthd;
	      else if( (*pit) == "sc_mthd" )
		ts << (*rit).sc_mthd;
	      else if( (*pit) == "cal_mthd" )
		ts << (*rit).cal_mthd;
	      else if( (*pit) == "cal_file" )
		ts << (*rit).cal_file;
	      else if( (*pit) == "rfi_mthd" )
		ts << (*rit).rfi_mthd;
	      else if( (*pit) == "ifr_mthd" )
		ts << (*rit).ifr_mthd;
              else
                ts << "INVALID";
            }
	    ts << endl;
          }
        }
      }
      ts.flush();
    }
  }
}








vector< string > GetFilenames ( int argc, char *argv[] )
{
  vector< string > filenames;

  for( int i = optind; i < argc; i ++ )
  {
    dirglob( &filenames, argv[i] );
  }

  return filenames;
}



void ProcessArchive( string filename )
{
  Reference::To< Archive > archive = Archive::load( filename );

  if( !archive )
    return;

  if( !keep_baseline )
    archive->remove_baseline();

  Interpreter preprocessor;
  preprocessor.set( archive );
  preprocessor.script( jobs );

  if( archive->get_state() != Signal::Stokes && (show_pol_frac || show_lin_frac || show_circ_frac || show_pa ) )
    archive->convert_state(Signal::Stokes);

  if( archive )
  {
    if( cal_parameters )
      CalParameters( archive );
    if( cmd_text )
      OutputDataAsText( archive );
    if( cmd_flux )
      Flux( archive );
  }
}


int main( int argc, char *argv[] ) try
{
  string args;
  args += HELP_KEY;
  args += CALIBRATOR_KEY;
  args += IBIN_KEY; args += ':';
  args += ICHAN_KEY; args += ':';
  args += ISUB_KEY; args += ':';
  args += PHASE_KEY; args += ':';
  args += FSCRUNCH_KEY;
  args += TSCRUNCH_KEY;
  args += PSCRUNCH_KEY;
  args += CENTRE_KEY;
  args += BSCRUNCH_KEY; args += ':';
  args += STOKES_FRACPOL_KEY;
  args += STOKES_FRACLIN_KEY;
  args += STOKES_FRACCIR_KEY;
  args += STOKES_POSANG_KEY;
  args += CALIBRATOR_KEY;
  args += BASELINE_KEY;
  args += PULSE_WIDTHS_KEY; args += ':';
  args += TEXT_KEY;
  args += TEXT_HEADERS_KEY;
  args += PER_SUBINT_KEY; args += ":";
  args += HISTORY_KEY; args += ":";

  vector<string> history_params;

  int i;
  while( ( i = getopt( argc, argv, args.c_str() ) ) != -1 )
  {
    switch( i )
    {

    case CALIBRATOR_KEY:
      cal_parameters = true;
      break;
    case BASELINE_KEY:
      keep_baseline = true;
      break;
    case TEXT_KEY:
      cmd_text = true;
      break;
    case TEXT_HEADERS_KEY:
      cmd_text = true;
      per_channel_headers = true;
      break;
    case PULSE_WIDTHS_KEY:
      if (sscanf(optarg, "%f", &dc) != 1)
      {
        cerr << "Invalid duty cycle" << endl;
        exit(-1);
      }
      cmd_flux = true;
      break;
    case HELP_KEY:
      Usage();
      break;
    case ICHAN_KEY:
      ichan = fromstring<int>( string(optarg) );
      break;
    case IBIN_KEY:
      ibin = fromstring<int>( string(optarg) );
      break;
    case ISUB_KEY:
      isub = fromstring<int>( string(optarg) );
      break;
    case PHASE_KEY:
      phase = fromstring<float>( string(optarg) );
      jobs.push_back( string("rotate ") + tostring<float>( phase ) );
      break;
    case FSCRUNCH_KEY:
      jobs.push_back( "fscrunch" );
      break;
    case TSCRUNCH_KEY:
      jobs.push_back( "tscrunch" );
      break;
    case PSCRUNCH_KEY:
      jobs.push_back( "pscrunch" );
      break;
    case BSCRUNCH_KEY:
      jobs.push_back( "bscrunch x" + string(optarg) );
      break;
    case CENTRE_KEY:
      jobs.push_back( "centre" );
      break;
    case STOKES_FRACPOL_KEY:
      show_pol_frac = true;
      break;
    case STOKES_FRACLIN_KEY:
      show_lin_frac = true;
      break;
    case STOKES_FRACCIR_KEY:
      show_circ_frac = true;
      break;
    case STOKES_POSANG_KEY:
      show_pa = true;
      break;
    case PER_SUBINT_KEY:
      cmd_subints = true;
      break;
    case HISTORY_KEY:
      cmd_history = true;
      separate (optarg, history_params, " ,");
      break;
    default:
      cerr << "Unknown option " << char(i) << endl;
      break;
    };
  }

  vector< string > filenames = GetFilenames( argc, argv );

  if( cal_parameters || cmd_text || cmd_flux )
  {

    for_each( filenames.begin(), filenames.end(), ProcessArchive );
  }

  if( cmd_subints )
    DisplaySubints( filenames );
  if( cmd_history )
    DisplayHistory( filenames, history_params );

  return 0;
}
catch (Error& error)
{
  cerr << error << endl;
  return -1;
}



