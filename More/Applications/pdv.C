//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Applications/pdv.C,v $
   $Revision: 1.29 $
   $Date: 2008/06/10 06:41:31 $
   $Author: jonathan_khoo $ */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_CFITSIO
#include "Pulsar/FITSArchive.h"
using Pulsar::FITSArchive;
#endif

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Interpreter.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/ProfileStats.h"

#include <Pulsar/ProcHistory.h>
#include <Pulsar/DigitiserStatistics.h>
#include <Pulsar/Integration.h>
#include <Pulsar/Pointing.h>

#include <table_stream.h>
#include <algorithm>
#include <functional>
#include <strutil.h>

#include "strutil.h"
#include "dirutil.h"
#include <tostring.h>

#include <iostream>
#include <vector>


const char HELP_KEY             = 'h';
const char IBIN_KEY             = 'b';
const char ICHAN_KEY            = 'n';
const char ISUB_KEY             = 'i';
const char PHASE_KEY            = 'r';
const char FSCRUNCH_KEY         = 'F';
const char TSCRUNCH_KEY         = 'T';
const char PSCRUNCH_KEY         = 'p';
const char CENTRE_KEY           = 'C';
const char BSCRUNCH_KEY         = 'B';
const char STOKES_FRACPOL_KEY   = 'x';
const char STOKES_FRACLIN_KEY   = 'y';
const char STOKES_FRACCIR_KEY   = 'z';
const char STOKES_POSANG_KEY    = 'Z';
const char CALIBRATOR_KEY       = 'c';
const char PULSE_WIDTHS_KEY     = 'f';
const char PULSE_FLUX_KEY       = 'I';
const char BASELINE_KEY         = 'R';
const char TEXT_KEY             = 't';
const char TEXT_HEADERS_KEY     = 'A';
const char PER_SUBINT_KEY       = 'S';
const char HISTORY_KEY          = 'H';
const char SNR_KEY              = 'N';




using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::ios;
using std::ostream;
using Pulsar::Archive;
using Pulsar::Profile;
using Pulsar::Integration;
using Pulsar::DigitiserStatistics;
using Pulsar::Pointing;
using Pulsar::Interpreter;
using Pulsar::ProcHistory;
using Pulsar::IntegrationOrder;




bool cmd_text = false;
bool cmd_flux = false;
bool cmd_flux2 = false;
bool cmd_subints = false;
bool cmd_history = false;
bool cmd_snr = false;
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
  "     pdv [options] filenames \n"
  "     pdv help [arg] \n"  "Where: \n"
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
  "   -" << PULSE_FLUX_KEY <<     "          print the mean flux density \n"
  "   -" << BASELINE_KEY <<       "          Do not remove baseline \n"
  "   -" << TEXT_KEY <<           "          Print out profiles as ASCII text \n"
  "   -" << TEXT_HEADERS_KEY <<   "          Print out profiles as ASCII text (with per channel headers) \n"
  "   -" << PER_SUBINT_KEY <<     " params   Print out per subint data (no params for argument list) \n"
  "   -" << HISTORY_KEY <<        " params   Print out the history table for the archive (no params for argument list) \n"
  "   -" << SNR_KEY <<            "          Print the S/N \n"
  " \n"
  "   For more detailed list of options use \"pdv -h param\", ie \"pdv -h S\" \n"
  "   for a full list of parameters that can be used with -S \n"
  << endl;
}



void DisplaySubintsUsage( void )
{
  cout <<
  "Usage: \n"
  "    pdv -S parameters \n"
  "Where: \n"
  "    Parameters are of the form param1,param2 or \"param1 param2\" \n"
  "    Parameter names are case insensitive \n"
  "Available Parameters: \n"
  "    INDEXVAL           Optionally used if INT_TYPE != TIME \n"
  "    TSUBINT            Length of subintegration \n"
#ifdef HAVE_CFITSIO
  "    OFFS_SUB           Offset from Start of subint centre \n"
#endif
  "    LST_SUB            LST at subint centre \n"
  "    RA_SUB             RA (J2000) at subint centre \n"
  "    DEC_SUB            Dec (J2000) at subint centre \n"
  "    GLON_SUB           [deg] Gal longitude at subint centre \n"
  "    GLAT_SUB           [deg] Gal latitude at subint centre \n"
  "    FD_ANG             [deg] Feed angle at subint centre \n"
  "    POS_ANG            [deg] Position angle of feed at subint centre \n"
  "    PAR_ANG            [deg] Parallactic angle at subint centre \n"
  "    TEL_AZ             [deg] Telescope azimuth at subint centre \n"
  "    TEL_ZEN            [deg] Telescope zenith angle at subint centre \n"
  "    S/N                Signal to noise ration for each subint\n"
  << endl;
}



void DisplayHistoryUsage( void )
{
  cout <<
  "Usage: \n"
  "    pdv -H parameters \n"
  "Where: \n"
  "    Parameters are of the form param1,param2 or \"param1 param2\" \n"
  "    Parameter names are case insensitive \n"
  "Available Parameters: \n"
  "    DATE_PRO           Processing date and time (UTC) \n"
  "    PROC_CMD           Processing program and command \n"
  "    SCALE              Units (FluxDen/RefFlux/Jansky) \n"
  "    POL_TYPE           Polarisation identifier \n"
  "    NSUB               Number of Sub-Integrations \n"
  "    NPOL               Number of polarisations \n"
  "    NBIN               Nr of bins per product (0 for SEARCH mode) \n"
  "    NBIN_PRD           Nr of bins per period \n"
  "    TBIN               Time per bin or sample \n"
  "    CTR_FREQ           Band centre frequency \n"
  "    NCHAN              Number of frequency channels \n"
  "    CHAN_BW            Channel bandwidth \n"
  "    PR_CORR            Projection correction applied \n"
  "    FD_CORR            Feed basis correction applied \n"
  "    RM_CORR            RM correction applied \n"
  "    DEDISP             Data dedispersed \n"
  "    DDS_MTHD           Dedispersion method \n"
  "    SC_MTHD            Scattered power correction method \n"
  "    CAL_MTHD           Calibration method \n"
  "    CAL_FILE           Name of gain calibration file \n"
  "    RFI_MTHD           RFI excision method \n"
  "    IFR_MTHD           Ionospheric Faraday rotation correction method \n"
  << endl;
}



void Header( Reference::To< Pulsar::Archive > archive )
{
  cout << "File: " << archive->get_filename()
  << " Src: " << archive->get_source()
  << " Nsub: " << archive->get_nsubint()
  << " Nch: " << archive->get_nchan()
  << " Npol: " << archive->get_npol()
  << " Nbin: " << archive->get_nbin()
  << " RMS: " << sqrt(archive->get_Profile(0,0,0)->baseline()->get_variance().get_value()) << endl;
}

void IntegrationHeader( Reference::To< Pulsar::Integration > intg )
{
  cout << "MJD(mid): " << intg->get_epoch().printdays(12);
  tostring_precision = 3;
  cout << " Tsub: " << tostring<double>( intg->get_duration() );
}

void OutputDataAsText( Reference::To< Pulsar::Archive > archive )
{
  int nsub = archive->get_nsubint();
  int npol = archive->get_npol();
  int nchn = archive->get_nchan();

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

      for (int s = fsub; s <= lsub; s++)
      {
        Integration* intg = archive->get_Integration(s);
        for (int c = fchan; c <= lchan; c++)
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
            cout << " Freq: " << tostring<double>( intg->get_centre_frequency( c ) );
            cout << " BW: " << intg->get_bandwidth() / nchn;
            cout << endl;
          }
          for (int b = fbin; b <= lbin; b++)
          {
            cout << s << " " << c << " " << b;
            for(int ipol=0; ipol<npol; ipol++)
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
			  if (show_pa) {
				  cout << " " << PAs[b].get_value();
				  cout << " " << sqrt(PAs[b].get_variance());
			  }
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

Estimate<float> flux2 (const Profile* profile)
{
  Pulsar::ProfileStats stats;

  stats.set_profile( profile );
  return stats.get_total() / profile->get_nbin();
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

  for (int s = fsub; s <= lsub; s++)
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

    for (int c = fchan; c <= lchan; c++)
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

  cout << "File\t\t\tSub\tChan\tPol\tMJD\tCentre Freq\tFlux\tUnit\t10\% Width\t50\% Width\tMJD"
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

  for (int s = fsub; s <= lsub; s++)
  {
    for (int c = fchan; c <= lchan; c++)
    {
      for (unsigned k = 0; k < archive->get_npol(); k++)
      {
        cout << archive->get_filename() << "\t";
        cout << s << "\t" << c << "\t" << k << "\t";
        cout.setf(ios::showpoint);
		cout << archive->start_time() << "\t";
		cout << archive->get_centre_frequency() << "\t";
        cout.setf(ios::showpoint);
        cout << flux(archive->get_Profile(s,k,c),dc, -1);
        if (archive->get_scale() == Signal::Jansky)
          cout << "\t" << "mJy";
        else
          cout << "\t" << "Arb";
        cout << "\t" << width(archive->get_Profile(s,k,c),junk, 10,dc);
        if (width(archive->get_Profile(s,k,c),junk, 10,dc) == 0)
          cout << "\t";
        cout << "\t" << width(archive->get_Profile(s,k,c),junk, 50,dc);

        cout << endl;
      }
    }
  }
}


void Flux2( Reference::To< Archive > archive )
{
  archive->convert_state (Signal::Intensity);

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

  for (int s = fsub; s <= lsub; s++)
  {
    for (int c = fchan; c <= lchan; c++)
    {
      cout << archive->get_filename() << "\t";
      cout << s << "\t" << c << "\t";
      cout.setf(ios::showpoint);

      Estimate<float> flux = flux2( archive->get_Profile(s,0,c) );
      cout << flux.get_value() << " " << flux.get_error();

      if (archive->get_scale() == Signal::Jansky)
        cout << "\t" << "mJy";

      cout << endl;
    }
  }
}


bool CheckPointing( Reference::To<Pointing> pointing, table_stream &ts )
{
  if( pointing )
    return true;

  ts << "INVALID";
  return false;
}



/**
 * DisplaySubints    Display a subint table header parameters.
 *
 * @param filenames  The files to read the data from
 * @param parameters The columns to display for each subint table. (assumed to be all uppercase).
 **/

void DisplaySubints( vector<string> filenames, vector<string> parameters )
{
  // If we don't have filenames or parameters, output a usage message

  if( filenames.size() == 0 || parameters.size() == 0 )
  {
    cerr << "No filenames given, or no parameters given." << endl;
    return;
  }

  vector<string>::iterator fit;
  for( fit = filenames.begin(); fit != filenames.end(); fit ++ )
  {
    // Load the archive
    Reference::To<Archive> data = Archive::load( (*fit) );
    if( !data )
    {
      cerr << "Failed to load archive " << (*fit) << endl;
      break;
    }

    cout << (*fit) << endl;

    table_stream ts( &cout );

    vector<string>::iterator pit;
    for( pit = parameters.begin(); pit != parameters.end(); pit ++ )
    {
      ts << (*pit);
    }
    ts << endl;

    // Get references to any extensions we may be interested in
    Reference::To<DigitiserStatistics> stats = data->get<DigitiserStatistics>();
    Reference::To<Pointing> pointing = data->get<Pointing>();

    int nsub = data->get_nsubint();

    for( int i = 0; i < nsub; i ++ )
    {
      try
      {
        Reference::To<Integration> integ = data->get_Integration( i );
        Reference::To<Pointing> pointing;
        Reference::To<IntegrationOrder> integ_order = data->get<IntegrationOrder>();

        if( integ )
          pointing = integ->get<Pointing>();

        vector<string>::iterator pit;
        for( pit = parameters.begin(); pit != parameters.end(); pit ++ )
        {
          // parameters from the SUBINT table
          if( (*pit) == "INDEXVAL" )
          {
            if( !integ_order )
              ts << "INVALID";
            else
              ts << tostring<double>( integ_order->get_Index(i) );
          }
          else if( (*pit) == "TSUBINT" )
          {
            if( !integ )
              ts << "INVALID";
            else
              ts << tostring<double>( integ->get_duration() );
          }

#ifdef HAVE_CFITSIO

          else if( (*pit) == "OFFS_SUB" )
          {
            Reference::To<FITSArchive> data_fits = dynamic_cast<FITSArchive*>( data.get() );
            if( ! data_fits )
              ts << "INVALID";
            else
              ts << tostring<double>( data_fits->get_offs_sub( i ) );
          }

#endif

          else if( (*pit) == "LST_SUB" && CheckPointing( pointing, ts ) )
            ts << tostring<double>( pointing->get_local_sidereal_time () );
          else if( (*pit) == "RA_SUB" && CheckPointing( pointing, ts ))
            ts << tostring<double>( pointing->get_right_ascension().getDegrees() );
          else if( (*pit) == "DEC_SUB" && CheckPointing( pointing, ts ))
            ts << tostring<double>( pointing->get_declination().getDegrees() );
          else if( (*pit) == "GLON_SUB" && CheckPointing( pointing, ts ))
            ts << tostring<double>( pointing->get_galactic_longitude().getDegrees() );
          else if( (*pit) == "GLAT_SUB" && CheckPointing( pointing, ts ))
            ts << tostring<double>( pointing->get_galactic_latitude().getDegrees() );
          else if( (*pit) == "FD_ANG" && CheckPointing( pointing, ts ))
            ts << tostring<double>( pointing->get_feed_angle().getDegrees() );
          else if( (*pit) == "POS_ANG" && CheckPointing( pointing, ts ))
            ts << tostring<double>( pointing->get_position_angle().getDegrees() );
          else if( (*pit) == "PAR_ANG" && CheckPointing( pointing, ts ))
            ts << tostring<double>( pointing->get_parallactic_angle().getDegrees() );
          else if( (*pit) == "TEL_AZ" && CheckPointing( pointing, ts ) )
            ts << tostring<double>( pointing->get_telescope_azimuth().getDegrees() );
          else if( (*pit) == "TEL_ZEN" && CheckPointing( pointing, ts ) )
            ts << tostring<double>( pointing->get_telescope_zenith().getDegrees() );
          else if( (*pit) == "S/N" )
          {
            ts << tostring<double>( integ->total()->get_Profile(0,0)->snr() );
          }


          // parameters from the DIG_CNTS table
          else if( (*pit) == "ATTEN" )
          {
            string atten_string;

            if( stats )
            {
              int num_atten = stats->rows[i].atten.size();
              if( num_atten > 0 )
              {
                for( int a = 0; a < num_atten-1; a ++ )
                  atten_string += tostring<float>( stats->rows[i].atten[a] ) + string(",");
                atten_string += tostring<float>( stats->rows[i].atten[num_atten-1] );
              }
            }
            else
              atten_string = "INVALID";
            ts << atten_string;
          }
        }
        ts << endl;
      }
      catch( Error e )
      {
        cerr << e << endl;
        break;
      }
    }

    ts.flush();
  }
}




/**
 * DisplayHistory     Display entries in the archives history tables.
 *
 * @param filenames   The archives to display.
 * @param params      The columns from the subint table to display. (assumed to be all lowercase)
 **/

void DisplayHistory( vector<string> filenames, vector<string> params )
{
  if( filenames.size() == 0 || params.size() == 0 )
  {
    cerr << "No filenames given, or no parameters given" << endl;
    return;
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
              if( (*pit) == "DATE_PRO" )
                ts << (*rit).date_pro;
              else if( (*pit) == "PROC_CMD" )
                ts << (*rit).proc_cmd;
              else if( (*pit) == "SCALE" )
                ts << (*rit).scale;
              else if( (*pit) == "POL_TYPE" )
                ts << (*rit).pol_type;
              else if( (*pit) == "NPOL" )
                ts << tostring<int>( (*rit).npol );
              else if( (*pit) == "NBIN" )
                ts << tostring<int>( (*rit).nbin );
              else if( (*pit) == "NSUB" )
                ts << tostring<int>( (*rit).nsub );
              else if( (*pit) == "NBIN_PRD" )
                ts << tostring<int>( (*rit).nbin_prd );
              else if( (*pit) == "TBIN" )
                ts << tostring<double>( (*rit).tbin );
              else if( (*pit) == "CTR_FREQ" )
                ts << tostring<double>( (*rit).ctr_freq );
              else if( (*pit) == "NCHAN" )
                ts << tostring<int>( (*rit).nchan );
              else if( (*pit) == "CHAN_BW" )
                ts << tostring<double>( (*rit).chan_bw );
              else if( (*pit) == "PR_CORR" )
                ts << tostring<int>( (*rit).pr_corr );
              else if( (*pit) == "FD_CORR" )
                ts << tostring<int>( (*rit).fd_corr );
              else if( (*pit) == "RM_CORR" )
                ts << tostring<int>( (*rit).rm_corr );
              else if( (*pit) == "DEDISP" )
                ts << tostring<int>( (*rit).dedisp );
              else if( (*pit) == "DDS_MTHD" )
                ts << (*rit).dds_mthd;
              else if( (*pit) == "SC_MTHD" )
                ts << (*rit).sc_mthd;
              else if( (*pit) == "CAL_MTHD" )
                ts << (*rit).cal_mthd;
              else if( (*pit) == "CAL_FILE" )
                ts << (*rit).cal_file;
              else if( (*pit) == "RFI_MTHD" )
                ts << (*rit).rfi_mthd;
              else if( (*pit) == "IFR_MTHD" )
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




/**
 * @brief Display a snr for each file
 * @param filenames vector of filenames to use
 *
 *        Use a table stream to neatly order output, iterate through the filenames
 *        take the total and use the current snr algorithm on the resulting profile.
 **/

void PrintSNR( vector<string> filenames )
{
  table_stream ts( &cout );

  ts << "FILE" << "S/N" << endl;

  vector<string>::iterator it;
  for( it = filenames.begin(); it != filenames.end(); it ++ )
  {
    try
    {
      Reference::To<Archive> archive = Archive::load( (*it) );

      string filename = archive->get_filename();

      double snr = archive->total()->get_Profile(0,0,0)->snr();

      ts << filename;
      ts << tostring<double>(snr);
      ts << endl;
    }
    catch( Error e )
    {
      cerr << "Failed to extract snr from archive " << (*it) << endl;
      cerr << e << endl;
    }
  }
  ts.flush();
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
try
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
    if( cmd_flux2 )
      Flux2( archive );
  }
}
catch( Error e )
{
  cerr << e << endl;
}


int main( int argc, char *argv[] ) try
{
  string args = "V";
  args += HELP_KEY; args += ':';
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
  //args += PULSE_WIDTHS_KEY; args += ':';
  args += PULSE_WIDTHS_KEY;
  args += PULSE_FLUX_KEY;
  args += TEXT_KEY;
  args += TEXT_HEADERS_KEY;
  args += PER_SUBINT_KEY; args += ":";
  args += HISTORY_KEY; args += ":";
  args += SNR_KEY;

  vector<string> history_params;
  vector<string> subint_params;

  int i;
  while( ( i = getopt( argc, argv, args.c_str() ) ) != -1 )
  {
    switch( i )
    {
    case '?':
      {
        if( optopt == 'h' )
        {
          Usage();
        }
      }
      break;
    case 'V':
      Pulsar::Archive::set_verbosity (3);
      break;
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
  	  dc = Pulsar::Profile::default_duty_cycle;
      cmd_flux = true;
      break;
    case PULSE_FLUX_KEY:
      cmd_flux2 = true;
      break;
    case HELP_KEY:
      if( string(optarg) == string("S") )
        DisplaySubintsUsage();
      else if( string(optarg) == string("H") )
        DisplayHistoryUsage();
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
      if( optarg != NULL )
        separate( optarg, subint_params, " ," );
      break;
    case HISTORY_KEY:
      cmd_history = true;
      if( optarg != NULL )
        separate (optarg, history_params, " ,");
      break;
    case SNR_KEY:
      cmd_snr = true;
      break;
    default:
      cerr << "Unknown option " << char(i) << endl;
      break;
    };
  }
  
  if( optind < argc )
  {
    string first_arg = argv[optind];
    if( first_arg == "help" )
    {
      if( optind == argc - 1 )
        Usage();
      else
      {
        char help_param = argv[optind+1][0];
        switch( help_param )
        {
          case 'H':
            DisplayHistoryUsage();
            break;
          case 'S':
            DisplaySubintsUsage();
            break;
        };
        exit(0);
      }
    }
  }

  vector< string > filenames = GetFilenames( argc, argv );

  if( cmd_snr )
    PrintSNR( filenames );

  if( cal_parameters || cmd_text || cmd_flux || cmd_flux2 )
  {
    for_each( filenames.begin(), filenames.end(), ProcessArchive );
  }

  transform( subint_params.begin(), subint_params.end(), subint_params.begin(), &uppercase );
  transform( history_params.begin(), history_params.end(), history_params.begin(), &uppercase );

  if( cmd_subints )
    DisplaySubints( filenames, subint_params );
  if( cmd_history )
    DisplayHistory( filenames, history_params );

  return 0;
}
catch (Error& error)
{
  cerr << error << endl;
  return -1;
}



