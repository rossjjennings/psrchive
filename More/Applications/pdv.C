//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Applications/pdv.C,v $
   $Revision: 1.2 $
   $Date: 2007/09/24 08:52:17 $
   $Author: straten $ */


#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include "Pulsar/PolnProfile.h"
#include "Pulsar/Profile.h"
#include "Pulsar/PhaseWeight.h"

#include "Pulsar/OnPulseThreshold.h"
#include "Pulsar/GaussianBaseline.h"
#include "Pulsar/BaselineWindow.h"

#include "strutil.h"
#include "dirutil.h"

#include <iostream>
#include <vector>


using namespace std;
using namespace Pulsar;


bool cmd_text = false;
bool cmd_flux = false;
bool cmd_poln = false;


// default duty cycle
float dc = 0.15;




void Usage( void )
{
  cout << 
    "A program for extracting archive data in text form \n"
    "Usage: \n"
    "     pdv [-f dc] [-t] filenames \n"
    "Where: \n"
    "   -f dcyc     Show pulse widths and mean flux density (mJy) \n"
    "               with baseline width dcyc \n"
    "   -p          Print polarization summary \n"
    "   -t          Print out profiles as ASCII text \n"
       << endl;
}




void OutputDataAsText( Reference::To< Pulsar::Archive > archive )
{
  unsigned nsub = archive->get_nsubint();
  unsigned nchn = archive->get_nchan();
  unsigned npol = archive->get_npol();
  unsigned nbin = archive->get_nbin();

  try
  {
    if( nsub > 0 )
    {
      archive->remove_baseline();

      cout << "Filename: " << archive->get_filename();
      cout << "  Nsub: " << nsub << "  Nchan: " << nchn <<
      "  Npol: " << npol << "  Nbin: " << nbin << endl;
      for (unsigned isub = 0; isub < nsub; isub++)
      {
        Integration* intg = archive->get_Integration(isub);
        for (unsigned ichn = 0; ichn < nchn; ichn++)
        {
          for (unsigned ibin = 0; ibin < nbin; ibin++)
          {
            cout << isub << " " << ichn << " " << ibin;
            for(unsigned ipol=0; ipol<npol; ipol++)
            {
              Profile *p = intg->get_Profile( ipol, ichn );
              cout << " " << p->get_amps()[ibin];
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




void Flux( Reference::To< Archive > archive )
{
  archive->centre();

  if (archive->get_npol() == 4)
    archive->convert_state (Signal::Stokes);

  archive->remove_baseline ();

  cout << "File\t\t\tSub\tChan\tPol\tFlux\tUnit\t10\% Width\t50\% Width"
  << endl;

  float junk;

  for (unsigned i = 0; i < archive->get_nsubint(); i++)
  {
    for (unsigned j = 0; j < archive->get_nchan(); j++)
    {
      for (unsigned k = 0; k < archive->get_npol(); k++)
      {
        cout << archive->get_filename() << "\t";
        cout << i << "\t" << j << "\t" << k << "\t";
        cout.setf(ios::showpoint);
        cout << flux(archive->get_Profile(i,k,j),dc, -1);
        if (archive->get_scale() == Signal::Jansky)
          cout << "\t" << "mJy";
        else
          cout << "\t" << "Arb";
        cout << "\t" << width(archive->get_Profile(i,k,j),junk, 10,dc);
        if (width(archive->get_Profile(i,k,j),junk, 10,dc) == 0)
          cout << "\t";
        cout << "\t" << width(archive->get_Profile(i,k,j),junk, 50,dc)
        << endl;
      }
    }
  }
}


Reference::To<OnPulseEstimator> onpulse;

Reference::To<BaselineEstimator> baseline;

void Poln( Reference::To< Archive > archive )
{
  archive->convert_state(Signal::Stokes);
  archive->remove_baseline();

  if (!onpulse)
    onpulse = new OnPulseThreshold;

  if (!baseline)
    baseline = new GaussianBaseline;

  PhaseWeight onmask;
  PhaseWeight offmask;

  for (unsigned isub = 0; isub < archive->get_nsubint(); isub++)
  {
    for (unsigned ichan = 0; ichan < archive->get_nchan(); ichan++)
    {

      Reference::To<PolnProfile> prof;
      prof = archive->get_Integration(isub)->new_PolnProfile(ichan);

      cout << archive->get_source();

      cout.setf(ios::showpoint);

      onpulse->set_Profile( prof->get_Profile(0) );
      onpulse->get_weight( onmask );

      Estimate<double> total_flux = onmask.get_mean ();
      cout << " <I>=" << total_flux.get_value();

      Profile linear;
      prof->get_linear( &linear );
      onmask.set_Profile( &linear );
      
      Estimate<double> linear_flux = onmask.get_mean ();
      cout << " <L>=" << linear_flux.get_value();

      // profile[3] = Stokes V
      onmask.set_Profile( prof->get_Profile(3) );

      Estimate<double> circular_flux = onmask.get_mean ();
      cout << " <V>=" << circular_flux.get_value();

      Profile circular;
      prof->get_circular( &circular );
      onmask.set_Profile( &circular );

      Estimate<double> abs_circular_flux = onmask.get_mean ();
      cout << " <|V|>=" << abs_circular_flux.get_value();

      baseline->set_Profile( prof->get_Profile(0) );
      baseline->get_weight( offmask );

      cout << " sigma=" << offmask.get_variance().get_value();

      cout << " ";
      if (archive->get_scale() == Signal::Jansky)
	cout << "mJy";
      else
	cout << "Arb";

      float junk = 0;

      double W10 = width( prof->get_Profile(0),junk, 10,dc);
      cout << " W10=" << W10 * 360.0 << "deg";

      double W50 = width( prof->get_Profile(0),junk, 50,dc);
      cout << " W50=" << W50 * 360.0 << "deg" << endl;

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

  if( archive )
  {
    if( cmd_text )
      OutputDataAsText( archive );
    if( cmd_flux )
      Flux( archive );
    if( cmd_poln )
      Poln( archive );
  }
}




int main( int argc, char *argv[] ) try {

  int i;

  while( ( i = getopt( argc, argv, "tf:ph" )) != -1 )
  {
    switch( i )
    {

    case 'p':
      cmd_poln = true;
      break;

    case 't':
      cmd_text = true;
      break;
    case 'f':
      if (sscanf(optarg, "%f", &dc) != 1)
      {
        cerr << "Invalid duty cycle" << endl;
        exit(-1);
      }
      cmd_flux = true;
      break;
    case 'h':
      Usage();
      return 0;
    };
  }

  vector< string > filenames = GetFilenames( argc, argv );
  for_each( filenames.begin(), filenames.end(), ProcessArchive );

  return 0;
}
 catch (Error& error) {
   cerr << error << endl;
   return -1;
 }



