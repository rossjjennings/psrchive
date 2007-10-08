/*
 * Copyright (c) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 */



#include "PavApp.h"
#include <tostring.h>
#include <templates.h>







PavApp::PavApp()
{
  have_colour = true;
  ipol = 0;
  fsub = 0, lsub = 0;
  isubint = 0;
  ichan = 0;
  rot_phase = 0.0;
  svp = true;
  publn = false;
  axes = true;
  labels = true;
  n1 = 1;
  n2 = 1;
  min_freq = 0.0;
  max_freq = 0.0;
  min_phase = 0.0;
  max_phase = 1.0;
  reference_position_angle = 0.0;
  position_angle_height = 0.25;
  border = 0.1;
  dark = false;
  y_max = 1.0;
  y_min = 0.0;
  truncate_amp = 0.0;
  colour_map = pgplot::ColourMap::Heat;

  cbppo = false;
  cbpao = false;
  cblpo = false;
  cblao = false;

  ronsub = 0;
}



void PavApp::PrintUsage( void )
{
  cout << "Preprocessing options:" << endl;
  cout << " -b scr    Bscrunch scr phase bins together" << endl;
  cout << " -C        Centre the profiles on phase zero" << endl;
  cout << " -c        Don't remove baseline before plotting" << endl;
  cout << " -d        Dedisperse all channels" << endl;
  cout << " -r phase  Rotate all profiles by phase (in turns)" << endl;
  cout << " -f scr    Fscrunch scr frequency channels together" << endl;
  cout << " -F        Fscrunch all frequency channels" << endl;
  cout << " -t src    Tscrunch scr Integrations together" << endl;
  cout << " -T        Tscrunch all Integrations" << endl;
  cout << " -p        Add all polarisations together" << endl;
  cout << " -x clip   Truncate data to range min*peak to max*peak" << endl;
  cout << endl;
  cout << "Configuration options:" << endl;
  cout << " -K dev    Manually specify a plot device" << endl;
  cout << " -M meta   Read a meta-file containing the files to use" << endl;
  cout << endl;
  cout << "Selection options:" << endl;
  cout << " -z x1,x2  Zoom to this pulse phase range" << endl;
  cout << " -k f1,f2  Zoom to this frequency range" << endl;
  cout << " -y s1,s2  Zoom to this subint range" << endl;
  cout << " -N x,y    Divide the window into x by y panels" << endl;
  cout << " -H chan   Select which frequency channel to display" << endl;
  cout << " -P pol    Select which polarization to display" << endl;
  cout << " -I subint Select which sub-integration to display" << endl;
  cout << endl;
  cout << "Plotting options:" << endl;
  cout << " -D        Plot a single profile (chan 0, poln 0, subint 0 by default)" << endl;
  cout << " -G        Plot an image of amplitude against frequency & phase" << endl;
  cout << " -Y        Plot colour map of sub-integrations against pulse phase" << endl;
  cout << " -S        Plot polarization parameters (I,L,V,PA)" << endl;
  cout << " -X        Plot cal amplitude and phase vs frequency channel" << endl;
  cout << " -B        Display off-pulse bandpass & channel weights" << endl;
  cout << " -R        Plot stacked sub-integration profiles" << endl;
  cout << " -m        Plot Poincare vector in spherical coordinates" << endl;
  cout << " -n        Plot S/N against frequency" << endl;
  cout << " -j        Display a simple dynamic spectrum image" << endl;
  cout << endl;
  cout << "Other plotting options:" << endl;
  cout << " --publn        Publication quality plot (B&W)  L dashed, V dots" << endl;
  cout << " --publnc       Publication quality plot (colour)" << endl;
  cout << " --cmap index   Select a colour map for PGIMAG style plots" << endl;
  cout << "                The available indices are: (maybe 4-6 not needed)" << endl;
  cout << "                  0 -> Greyscale" << endl;
  cout << "                  1 -> Inverse Greyscale" << endl;
  cout << "                  2 -> Heat" << endl;
  cout << "                  3 -> Cold" << endl;
  cout << "                  4 -> Plasma" << endl;
  cout << "                  5 -> Forest" << endl;
  cout << "                  6 -> Alien Glow" << endl;
  cout << "                  7 -> Test" << endl;
  cout << endl;
  cout << " --plot_qu      Plot Stokes Q and Stokes U in '-S' option instead of degree of linear" << endl;
  cout << endl;
  cout << "Integration re-ordering (nsub = final # of subints) (used with pav Y for binary pulsars):" << endl;
  cout << " --convert_binphsperi   nsub" << endl;
  cout << " --convert_binphsasc    nsub" << endl;
  cout << " --convert_binlngperi   nsub" << endl;
  cout << " --convert_binlngasc    nsub" << endl;
  cout << endl;
  cout << "Utility options:" << endl;
  cout << " -a              Print available plugin information" << endl;
  cout << " -h              Display this useful help page" << endl;
  cout << " -i              Show revision information" << endl;
  cout << " -v              Verbose output" << endl;
  cout << " -V              Very verbose output" << endl;
  cout << endl << endl;
}






//! Plot an off-pulse bandpass
void PavApp::bandpass (const Archive* data)
{
  Reference::To<Pulsar::Archive> copy = data->clone();

  // Define a vector to hold the bandpass
  vector<double> mean;
  mean.resize(copy->get_nchan());

  // Define a vector to hold the weights array
  vector<float> weights;
  weights.resize(copy->get_nchan());

  float phase = 0.0;
  Pulsar::Profile* profile = 0;


  for (unsigned i = 0; i < copy->get_nchan(); i++)
  {
    profile = copy->get_Integration(isubint)->get_Profile(ipol, i);
    phase = profile->find_min_phase();
    mean[i] = profile->mean(phase);
    weights[i] = profile->get_weight();
  }

  // Plot the bandpass

  double min=0.0, max=0.0;
  // d_minmaxval ( mean, min, max );
  minmax( mean, min, max );

  cpgsvp(0.1, 0.9, 0.4, 0.9);
  cpgswin (0,copy->get_nchan(),min-(min/20.0),max+(max/20));

  cpgslw(3);
  cpgsci(2);
  cpgsch(1.0);

  cpgmove(0, mean[0]);
  for (unsigned i = 1; i < copy->get_nchan(); i++)
  {
    cpgdraw(i, mean[i]);
  }

  cpgslw(1);
  cpgsci(1);

  float major = 5;
  if (copy->get_nchan() > 50)
    major = 10;
  if (copy->get_nchan() > 200)
    major = 50;
  if (copy->get_nchan() > 500)
    major = 100;

  cpgbox("BCINTS", major, 10, "BCINTS", 0.0, 0);

  string title = "Bandpass: " + data->get_filename();
  cpglab("Frequency Channel", "Amplitude (Arbitrary)", title.c_str());

  // Plot the weights array

  float fmin=0.0, fmax=0.0;
  // minmaxval (weights, fmin, fmax);
  minmax( weights, fmin, fmax );

  cpgsvp(0.1, 0.9, 0.05, 0.3);
  cpgswin (0,copy->get_nchan(),fmin-(fmin/20.0),fmax+(fmax/20));

  cpgsci(6);
  cpgsch(0.5);
  for (unsigned i = 1; i < copy->get_nchan(); i++)
  {
    cpgpt1(i, weights[i], 4);
  }

  cpgsci(1);
  cpgsch(1.0);
  cpgbox("", 0.0, 0, "NT", 0.0, 0);
  cpglab("", "Weight", "");
  cpgsch(1.0);
}







void PavApp::spherical ( const Profile* I,
                         const Profile* Q,
                         const Profile* U,
                         const Profile* V,
                         bool calibrated )
{
  const float* q = Q->get_amps();
  const float* u = U->get_amps();
  const float* v = V->get_amps();

  unsigned nbin = I->get_nbin();

  vector<float> phase (nbin);
  vector<float> orientation (nbin);
  vector<float> ellipticity (nbin);
  vector<float> polarization (nbin);
  vector<float> linear (nbin);

  double reference_orientation_turns = reference_position_angle / 180.0;

  for (unsigned ibin=0; ibin<nbin; ibin++)
  {

    phase[ibin] = float(ibin)/float(nbin-1) - rot_phase;
    // ensure that phase runs from 0 to 1
    phase[ibin] -= floor (phase[ibin]);

    float Lsq = q[ibin]*q[ibin] + u[ibin]*u[ibin];
    float Psq = Lsq + v[ibin]*v[ibin];

    linear[ibin] = sqrt (Lsq);
    polarization[ibin] = sqrt (Psq);

    double orientation_turns = atan2 (u[ibin], q[ibin]) / (2.0*M_PI);
    orientation_turns -= reference_orientation_turns;
    orientation_turns -= floor (orientation_turns);

    orientation[ibin] = 180.0 * orientation_turns;

  }

  float baseline_phase = I->find_min_phase();
  Profile temp_profile;

  temp_profile.set_amps (linear);
  temp_profile -= temp_profile.mean (baseline_phase);
  temp_profile.get_amps (linear);

  temp_profile.set_amps (polarization);
  temp_profile -= temp_profile.mean (baseline_phase);

  for (unsigned ibin=0; ibin<nbin; ibin++)
    ellipticity[ibin] = 90.0/M_PI * atan2 (v[ibin], linear[ibin]);

  float x1, x2, y1, y2;
  cpgsci(1);
  cpgqvp (0, &x1, &x2, &y1, &y2);

  float height1 = position_angle_height*(y2-y1);

  // //////////////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////////////
  // plot the orientation
  cpgsvp (x1, x2, y2-height1, y2);

  cpgswin (min_phase, max_phase, 0.001, 179.999);
  cpgsch(1.2);
  cpgbox("bcst",0,0,"bcnvst",60,3);

  if (labels)
  {
    cpgsch(1.5);
    cpgmtxt("L",2.5,.5,.5,"\\gh (deg.)");
  }

  cpgpt (nbin, &phase[0], &orientation[0], 1);

  // //////////////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////////////
  // plot the ellipticity
  cpgsvp (x1, x2, y2-height1*2.0, y2-height1);

  cpgswin (min_phase, max_phase, -44.999, 44.999);
  cpgsch(1.2);
  cpgbox("bcst",0,0,"bcnvst",30,3);

  if (labels)
  {
    cpgsch(1.5);
    cpgmtxt("L",2.5,.5,.5,"\\ge (deg.)");
  }

  cpgpt (nbin, &phase[0], &ellipticity[0], 1);

  bool logarithmic = true;

  Reference::To<Profile> tempI;

  if (logarithmic)
  {

    double variance;
    I->stats (baseline_phase, 0, &variance);

    float rms = sqrt(variance);
    float log_rms = log(rms)/log(10.0);

    tempI = new Profile (*I);
    tempI->logarithm (10.0, rms);
    *tempI -= log_rms;
    I = tempI;

    temp_profile.logarithm (10.0, rms);
    temp_profile -= log_rms;

  }

  // //////////////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////////////
  // set the viewport to plot the rest
  cpgsvp (x1, x2, y1, y2-height1*2.0);

  float ymin = 0.0;
  float ymax = I->max() * y_max;
  float ybuf = border * (ymax-ymin);

  cpgswin (min_phase, max_phase, ymin, ymax+ybuf);


  // //////////////////////////////////////////////////////////////////////
  // plot total intensity
  if (dark)
  {
    // white, solid line
    cpgsci(1);
    cpgsls(1);
  }
  else
  {
    // black, dotted line
    cpgsci(1);
    cpgsls(1);
  }

  I->get_amps (linear);
  cpgpt (nbin, &phase[0], &linear[0], 1);


  // I->draw (rot_phase);


  // //////////////////////////////////////////////////////////////////////
  // plot total polarization
  if (dark)
  {
    // red, solid line
    cpgsci(2);
    cpgsls(1);
  }
  else
  {
    // red, dashed line
    cpgsci(2);
    cpgsls(2);
  }

  temp_profile.get_amps (linear);
  cpgpt (nbin, &phase[0], &linear[0], 1);


  // //////////////////////////////////////////////////////////////////////
  // plot some labels

  cpgsci(1);
  cpgsls(1);

  cpgsch(1.2);
  cpgbox("bcnst",0.0,0,"bcnlstv",0.0,0);

  cpgsch(1.5);
  if (labels)
  {
    cpgmtxt("L",2.5,.5,.5,"Flux (\\fi\\gs\\fr\\d0\\u)");
    cpgmtxt("B",2.5,.5,.5,"Pulse Phase");
  }

  cpgsch(1);

  // restore the viewport
  cpgsvp (x1, x2, y1, y2);

}




void PavApp::spherical_wrapper (const Archive* data)
{
  Reference::To<Archive> clone = data->clone();

  // convert to Stokes parameters
  clone->convert_state (Signal::Stokes);
  // remove the baseline
  clone->remove_baseline();

  data = clone;

  const Profile* I = data->get_Profile (isubint, 0, ichan);
  const Profile* Q = data->get_Profile (isubint, 1, ichan);
  const Profile* U = data->get_Profile (isubint, 2, ichan);
  const Profile* V = data->get_Profile (isubint, 3, ichan);

  spherical (I,Q,U,V,data->get_scale() == Signal::Jansky);
}




/**
 * SetTokesPlotToQU
 *
 * DOES     - in response to the --plot_qu option, find any stokes cylindrical plots in the list
 *            and set the flux values to plot to be IQUV, give them different colours and if printing
 *            use different line styles
 * RECEIVES - A vector of plots to set the option on
 * RETURNS  - Nothing
 * THROWS   - Nothing
 * TODO     - Nothing
 **/

void PavApp::SetStokesPlotToQU( vector< Reference::To<Plot> > &plots )
{
  vector< Reference::To<Plot> >::iterator it;
  for( it = plots.begin(); it != plots.end(); it ++ )
  {
    Reference::To<StokesCylindrical> sp = dynamic_cast<StokesCylindrical*>( (*it).get() );
    if( sp )
    {
      Reference::To<TextInterface::Parser> ti_base = sp->get_interface();
      Reference::To<StokesCylindrical::Interface> ti = dynamic_cast<StokesCylindrical::Interface*>( ti_base.get() );

      if( ti )
      {
        ti->process( "flux:val=IQUV" );
        
        if( !have_colour )
	{
	  ti->process( "flux:ci=1111" );
          ti->process( "flux:ls=1234" );
	}
        else
	{
	  ti->process( "flux:ci=1234" );
          ti->process( "flux:ls=1111" );
	}
      }
    }
  }
}



void PavApp::SetPhaseZoom( double min_phase, double max_phase, vector< Reference::To<Plot> > &plots )
{
  string range_cmd = string("x:range=(") +
                     tostring<double>( min_phase ) +
                     string(",") +
                     tostring<double>( max_phase ) +
                     string( ")");

  vector< Reference::To<Plot> >::iterator it;
  for( it = plots.begin(); it != plots.end(); it ++ )
    (*it)->configure( range_cmd );
}



void PavApp::SetFreqZoom( double min_freq, double max_freq, vector< Reference::To<Plot> > &plots, Reference::To<Archive> archive )
{
  // get the centre frequency and bandwidth

  double ctr_freq = archive->get_centre_frequency();
  double bw = archive->get_bandwidth();
  double bw_abs = bw;

  if( bw < 0 )
  {
    bw_abs = 0 - bw;
  }

  double actual_min = ctr_freq - bw_abs / 2.0;

  // convert the min and max freq values to (0-1)

  min_freq -= actual_min;
  max_freq -= actual_min;

  min_freq /= bw_abs;
  max_freq /= bw_abs;

  if( bw < 0 )
  {
    double tmp = 1 - min_freq;
    min_freq = 1 - max_freq;
    max_freq = tmp;
  }

  // construct the command to set the range, the same for all plots (for current plots)

  string range_cmd = "y:range=(";
  range_cmd += tostring<double>( min_freq );
  range_cmd += string(",");
  range_cmd += tostring<double>( max_freq );
  range_cmd += string(")" );

  vector< Reference::To<Plot> >::iterator it;
  for( it = plots.begin(); it != plots.end(); it ++ )
    (*it)->configure( range_cmd );
}






int PavApp::run( int argc, char *argv[] )
{
  tostring_places = true;
  tostring_precision = 3;
  
  vector< string > filenames;

  // name of file containing list of Archive filenames
  char* metafile = NULL;

  // PGPLOT device name
  string plot_device = "/xs";

  // Plot classes to be used
  vector< Reference::To< Plot > > plots;

  // Options to be set
  vector<string> options;

  // Preprocessing jobs
  vector<string> jobs;

  // verbosity
  bool verbose = false;

  // Available plots
  static PlotFactory factory;

  // Keep the baseline before plotting
  bool keep_baseline = false;

  bool plot_bandpass = false;

  bool plot_line_phase_subints = false;

  bool plot_spherical = false;

  bool plot_snr_spectrum = false;

  bool plot_qu = false;

  bool perform_test = false;

  int option_index;

  const int PLOT_QU          = 1001;
  const int CMAP_IND         = 1002;
  const int BINPHSP          = 1003;
  const int BINPHSA          = 1004;
  const int BINPHLP          = 1005;
  const int BINPHLA          = 1006;
  const int PUBLN            = 1007;
  const int PUBLNC           = 1008;

  static struct option long_options[] =
    {
      { "convert_binphsperi", 1, 0, BINPHSP },
      { "convert_binphsasc",  1, 0, BINPHSA },
      { "convert_binlngperi", 1, 0, BINPHLP },
      { "convert_binlngasc",  1, 0, BINPHLA },
      { "plot_qu",            0, 0, PLOT_QU },
      { "cmap",               1, 0, CMAP_IND },
      { "publn",              0, 0, PUBLN },
      { "publnc",             0, 0, PUBLNC },
      { 0,0,0,0 }
    };

    string top_label = "above:c";
    string top_left_label = "below:l";
    string clip_string = "y:range";

  char valid_args[] = "z:hb:M:K:DCcdr:f:Ft:TGYSXBRmnjpP:y:H:I:N:k:ivVax:";

  int c = '\0';
  while( (c = getopt_long( argc, argv, valid_args, long_options, &option_index )) != -1 )
  {
    switch( c )
    {
    case 'a':
      Pulsar::Archive::agent_report ();
      return 0;
    case 'h':
      PrintUsage();
      break;
    case 'b':
      {
        jobs.push_back( "bscrunch" );
        break;
      }
    case 'i':
      cout << "pav VERSION $Id: PavApp.C,v 1.9 2007/10/08 06:36:12 nopeer Exp $" << endl << endl;
      return 0;
      break;
    case 'M':
      metafile = optarg;
      break;
    case 'K':
      plot_device = optarg;
      break;
    case 'D':
      plots.push_back( factory.construct( "flux" ) );
      break;
    case 'C':
      jobs.push_back( "centre" );
      break;
    case 'c':
      keep_baseline = true;
      break;
    case 'd':
      jobs.push_back( "dedisperse" );
      break;
    case 'V':
      Pulsar::Archive::set_verbosity (3);
      break;
    case 'v':
      Pulsar::Archive::set_verbosity (2);
      break;
    case 'r':
      {
        ostringstream s_job;
        s_job << "rotate " << optarg;
        jobs.push_back( s_job.str() );
        break;
      }
    case 'f':
      {
        ostringstream s_job;
        s_job << "fscrunch " << optarg;
        jobs.push_back( s_job.str() );
        break;
      }
    case 'F':
      jobs.push_back( "fscrunch" );
      break;
    case 't':
      {
        ostringstream s_job;
        s_job << "tscrunch " << optarg;
        jobs.push_back( s_job.str() );
        break;
      }
    case 'T':
      jobs.push_back( "tscrunch" );
      break;
    case 'p':
      jobs.push_back( "pscrunch" );
      break;
    case 'G':
      plots.push_back( factory.construct( "freq" ) );
      break;
    case 'Y':
      //keep_baseline = true;
      plots.push_back( factory.construct( "time" ) );
      break;
    case 'S':
      top_label = "pa:above:c";
      top_left_label = "pa:below:l";
      clip_string = "flux:y:range";
      options.push_back( "pa:mark=dot+tick" );
      plots.push_back( factory.construct( "S" ) );
      break;
    case 'X':
      keep_baseline = true;
      plots.push_back( factory.construct( "X" ) );
      break;
    case 'B':
      plot_bandpass = true;
      keep_baseline = true;
      break;
    case 'R':
      //keep_baseline = true;
      plots.push_back( factory.construct( "line" ) );
      break;
    case 'm':
      plot_spherical = true;
      break;
    case 'n':
      plots.push_back( factory.construct("snrspec") );
      keep_baseline = true;
      break;
    case 'j':
      {
        plots.push_back( factory.construct( "dspec" ) );
        TextInterface::Parser *ti = plots.back()->get_interface();
        keep_baseline = true;
      }
      break;
    case 'P':
      ipol = fromstring<int>( optarg );
      options.push_back( string("pol=") + string(optarg) );
      break;
    case 'y':
      {
        string s1,s2;
        string_split( optarg, s1, s2, "," );
        fsub = fromstring<unsigned int>( s1 );
        lsub = fromstring<unsigned int>( s2 );
        options.push_back( string("fsub=" ) + s1 );
        options.push_back( string("lsub=" ) + s2 );
      }
      break;
    case 'H':
      ichan = fromstring<int>( optarg );
      options.push_back( string("chan=") + string( optarg ) );
      break;
    case 'I':
      isubint = fromstring<unsigned int>( optarg );
      options.push_back( string("subint=" ) + string( optarg ) );
      break;
    case 'N':
      {
        string s1,s2;
        string_split( optarg, s1, s2, "," );
        n1 = fromstring<unsigned int>( s1 );
        n2 = fromstring<unsigned int>( s2 );
      }
      break;
    case 'k':
      {
        string s1, s2;
        string_split( optarg, s1, s2, "," );
        min_freq = fromstring<double>( s1 );
        max_freq = fromstring<double>( s2 );
      }
      break;
    case 'z':
      {
        string s1, s2;
        string_split( optarg, s1, s2, "," );
        min_phase = fromstring<double>( s1 );
        max_phase = fromstring<double>( s2 );
      }
      break;
    case PLOT_QU:
      plot_qu = true;
      break;
    case CMAP_IND:
      switch( fromstring< unsigned int>( optarg ) )
      {
      case 0:
        colour_map = pgplot::ColourMap::GreyScale;
        break;
      case 1:
        colour_map = pgplot::ColourMap::Inverse;
        break;
      case 2:
        colour_map = pgplot::ColourMap::Heat;
        break;
      case 3:
        colour_map = pgplot::ColourMap::Cold;
        break;
      case 4:
        colour_map = pgplot::ColourMap::Plasma;
        break;
      case 5:
        colour_map = pgplot::ColourMap::Forest;
        break;
      case 6:
        colour_map = pgplot::ColourMap::AlienGlow;
        break;
      case 7:
        colour_map = pgplot::ColourMap::Test;
        break;
      default:
        cerr << "Unknown colour map, use (0-7)" << endl;
      };
      break;
    case BINPHLA:
      cbpao = true;
      ronsub = fromstring<unsigned int>( optarg );
      break;
    case BINPHLP:
      cbppo = true;
      ronsub = fromstring<unsigned int>( optarg );
      break;
    case BINPHSA:
      cblao = true;
      ronsub = fromstring<unsigned int>( optarg );
      break;
    case BINPHSP:
      cblpo = true;
      ronsub = fromstring<unsigned int>( optarg );
      break;
    case PUBLN:
      publn = true;
      options.push_back( "set=pub" );
      break;
    case PUBLNC:
      publnc = true;
      options.push_back( "set=pubc" );
      break;
    case 'x':
      string s1,s2;
      string_split( optarg, s1, s2, "," );
      clip_string += string("=(") + s1 + string(",") + s2 + ")";
      break;
    };
  }

  // Set the top label
  // options.push_back( "$name $file. Freq: $freq MHz BW: $bw Length: $length S/N: >snr" );
  
  if( !publn && !publnc )
  {
      options.push_back( top_label + string( "=$name $file. Freq: $freq MHz BW: $bw Length: $length S/N: $snr" ) );
      options.push_back( top_left_label + string( "=" ) );
  }
  
  if( clip_string != "y:clip" )
  {
    options.push_back( clip_string );
  }

  if (plots.empty() && !plot_bandpass && !plot_spherical && !plot_snr_spectrum
      && !perform_test )
  {
    cout << "pav: please choose at least one plot style" << endl;
    return -1;
  }

  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  if (filenames.empty())
  {
    cout << "pav: please specify filename[s]" << endl;
    return -1;
  }

  if (cpgopen(plot_device.c_str()) < 0)
  {
    cout << "pav: Could not open plot device" << endl;
    return -1;
  }
  else
  {
    int first_index, last_index;
    cpgqcol( &first_index, &last_index );
    if( first_index == last_index )
      have_colour = false;
  }

  Interpreter preprocessor;

  if (n1 > 1 || n2 > 1)
    cpgsubp(n1,n2);

  for (unsigned ifile=0; ifile < filenames.size(); ifile++)
    try
    {

      Reference::To<Archive> archive;
      archive = Archive::load( filenames[ifile] );

      if (jobs.size())
      {
        if (verbose)
          cerr << "pav: preprocessing " << filenames[ifile] << endl;
        preprocessor.set(archive);
        preprocessor.script(jobs);
      }

      // set options for all the plots
      if( plots.size() )
      {
        vector< Reference::To<Plot> >::iterator it;
        for( it = plots.begin(); it != plots.end(); it ++ )
        {
          vector<string>::iterator oit;
          for( oit = options.begin(); oit != options.end(); oit ++ )
          {
            (*it)->configure( (*oit) );
          }
        }
      }

      if( !keep_baseline )
        archive->remove_baseline();

      if (verbose)
        cerr << "pav: plotting " << filenames[ifile] << endl;

      // if we are plotting qu with -S, go through each of the plots and find the stokes plot
      // and set it to plot QU
      if( plot_qu )
      {
        SetStokesPlotToQU( plots );
      }


      // TODO check this.
      if( !have_colour && !plot_qu )
      {
        vector< Reference::To<Plot> >::iterator it;
        for( it = plots.begin(); it != plots.end(); it ++ )
        {
          Plot *ptr = (*it).get();
          Reference::To<StokesCylindrical> p = dynamic_cast<StokesCylindrical*>( ptr );
          if( p )
          {
            //p->set_printing( true );
          }
        }
      }

      SetPhaseZoom( min_phase, max_phase, plots );

      if( min_freq != max_freq )
      {
        SetFreqZoom( min_freq, max_freq, plots, archive );
      }


      pgplot::ColourMap cmap;
      cmap.set_name ( colour_map );
      cmap.apply();



      if (cbppo)
      {
        Pulsar::IntegrationOrder* myio = new Pulsar::PeriastronOrder();
        archive->add_extension(myio);
        myio->organise(archive, ronsub);
      }

      if (cbpao)
      {
        Pulsar::IntegrationOrder* myio = new Pulsar::BinaryPhaseOrder();
        archive->add_extension(myio);
        myio->organise(archive, ronsub);
      }

      if (cblpo)
      {
        Pulsar::IntegrationOrder* myio = new Pulsar::BinLngPeriOrder();
        archive->add_extension(myio);
        myio->organise(archive, ronsub);
      }

      if (cblao)
      {
        Pulsar::IntegrationOrder* myio = new Pulsar::BinLngAscOrder();
        archive->add_extension(myio);
        myio->organise(archive, ronsub);
      }


      if( plot_bandpass )
      {
        bandpass( archive );
      }
      else if ( plot_spherical )
      {
        spherical_wrapper( archive );
      }
      else
      {
        for (unsigned iplot=0; iplot < plots.size(); iplot++)
        {
          cpgpage ();
          plots[iplot]->plot (archive);
        }
      }
    }
    catch( Error e )
    {
      cerr << "Exception occured " << e << endl;
    }

  cpgclos();


  return 0;
}



