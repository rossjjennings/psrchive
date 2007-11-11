/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "PavApp.h"
#include <tostring.h>
#include <templates.h>
#include <Pulsar/StokesCylindrical.h>
#include <Pulsar/BandpassChannelWeightPlot.h>
#include <Pulsar/StokesSpherical.h>
#include <limits>








/**
 * Constructor
 *
 *  DOES     - Inititialization
 *  RECEIVES - Nothing
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

PavApp::PavApp()
{
  have_colour = false;
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



/**
 * PrintUsage
 *
 *  DOES     - Display the help text to stdout
 *  RECEIVES - Nothing
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

void PavApp::PrintUsage( void )
{
  cout << "Preprocessing options:" << endl;
  cout << " -b scr    Bscrunch scr phase bins together" << endl;
  cout << " -C        Centre the profiles on phase zero" << endl;
  cout << " -d        Dedisperse all channels" << endl;
  cout << " -r phase  Rotate all profiles by phase (in turns)" << endl;
  cout << " -f scr    Fscrunch scr frequency channels together" << endl;
  cout << " -F        Fscrunch all frequency channels" << endl;
  cout << " -t src    Tscrunch scr Integrations together" << endl;
  cout << " -T        Tscrunch all Integrations" << endl;
  cout << " -p        Add all polarisations together" << endl;
  cout << " -x  max   Zoom into the data between 0 and max (normalized coords)" << endl;
  cout << endl;
  cout << "Configuration options:" << endl;
  cout << " -g dev    Manually specify a plot device" << endl;
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
  cout << " -B        Display off-pulse bandpass & channel weights" << endl;
  cout << " -D        Plot a single profile (chan 0, poln 0, subint 0 by default)" << endl;
  cout << " -G        Plot an image of amplitude against frequency & phase" << endl;
  cout << " -K        Plot Digitiser Statistics" << endl;
  cout << " -j        Display a simple dynamic spectrum image" << endl;
  cout << " -m        Plot Poincare vector in spherical coordinates" << endl;
  cout << " -n        Plot S/N against frequency" << endl;
  cout << " -R        Plot stacked sub-integration profiles" << endl;
  cout << " -S        Plot polarization parameters (I,L,V,PA)" << endl;
  cout << " -A        Plot Digitiser Counts histogram" << endl;
  cout << " -X        Plot cal amplitude and phase vs frequency channel" << endl;
  cout << " -Y        Plot colour map of sub-integrations against pulse phase" << endl;
  cout << endl;
  cout << "Other plotting options:" << endl;
  cout << " --publn        Publication quality plot (B&W)  L dashed, V dots" << endl;
  cout << " --publnc       Publication quality plot (keep in colour if device supports it)" << endl;
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





/**
 * spherical
 *
 *  DOES     - Draws a spherical plot
 *  RECEIVES - N/A
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Implement this as a Plot derived class
 **/

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



/**
 * spherical_wrapper
 *
 *  DOES     - calls the spherical function to do a spherical plot
 *  RECEIVES - Nothing
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Implement this as a Plot derived class.
 **/

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
 * SetPhaseZoom
 *
 *   DOES     - creates a command string that sets the x zoom in normalised coords.
 *   RECEIVES - Nothing
 *   RETURNS  - Nothing
 *   THROWS   - Nothing
 *   TODO     - Nothing
 **/

void PavApp::SetPhaseZoom( double min_phase, double max_phase  )
{
  string range_cmd = string("x:range=(") +
                     tostring< double >(min_phase ) +
                     string(",") +
                     tostring< double >( max_phase ) +
                     string( ")");

  SetPlotOptions<Plot>( range_cmd );
}



/**
 * SetFreqZoom
 *
 *  DOES     - creates a command string that sets the y zoom in normalised coords based
 *             on a target frequency range in Mhz.
 *  RECEIVES - Nothing
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

void PavApp::SetFreqZoom( double min_freq, double max_freq )
{
  for( int i = 0; i < plots.size(); i ++ )
  {
    Reference::To<Archive> archive = plots[i].archive;

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

    SetPlotOptions<Plot>( range_cmd );
  }
}



/**
 * PavSpecificOptions
 *
 *  DOES     - Sets all the options that are only for pav, ie blanks out labels that we don't want to see here
 *             but still want them present in psrplot and other programs.
 *  RECEIVES - Nothing
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

void PavApp::PavSpecificOptions( void )
{
  SetPlotOptions<BandpassChannelWeightPlot>( "band:below:l=$name" );
  SetPlotOptions<BandpassChannelWeightPlot>( "band:below:r=$freq MHz" );

  SetPlotOptions<StokesCylindrical>( "pa:below:l=" );
  SetPlotOptions<StokesCylindrical>( "flux:below:l=" );
  SetPlotOptions<StokesCylindrical>( "flux:y:buf=0.07" );
  
  SetPlotOptions<StokesSpherical>( "flux:below:l=" );
}



/**
 * CreatePlotsList
 *
 *  DOES     - given lists of filenames and plot identifiers, create a plotlist
 *  RECEIVES - vector of filenames, vector of plot ids
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

void PavApp::CreatePlotsList( vector< string > filenames,   vector< string > plot_ids )
{
  PlotFactory factory;

  for( int i = 0; i < filenames.size(); i ++ )
  {
    try
    {
      FilePlots new_fplot;
      new_fplot.filename = filenames[i];
      for( int p = 0; p < plot_ids.size(); p ++ )
      {
        new_fplot.plots.push_back( factory.construct( plot_ids[p] ) );
      }
      new_fplot.archive = Archive::load( filenames[i] );
      plots.push_back( new_fplot );
    }
    catch ( Error e )
    {
      cerr << "Unable to load archive " << filenames[i] << endl;
      cerr << "The following exception was encountered" << endl;
      cerr << e << endl;
    }
  }
}



/**
 * SetCmdLineOptions
 *
 *  DOES     - Set all of the options given on all the plots
 *  RECEIVES - vector of option strings
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

void PavApp::SetCmdLineOptions( vector< string > options )
{
  vector<string>::iterator it;
  for( it = options.begin(); it != options.end(); it ++ )
    SetPlotOptions<Plot>( (*it) );
}



/**
 * CheckColour
 *
 *  DOES     - Probe the device to determine if it suppots colour.
 *  RECEIVES - Nothing
 *  RETURNS  - Boolean true for colour support, false otherwise
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

bool PavApp::CheckColour( void )
{
  bool have_colour = false;

  int first_index, last_index;
  cpgqcol( &first_index, &last_index );
  if( first_index != last_index )
  {
    float red,green,blue;
    cpgqcr( 2, &red, &green, &blue );
    if( !( red == green && green == blue ) )
      have_colour = true;
  }

  return have_colour;
}





/**
 * run
 *
 *  DOES     - equivelent of main, so testing programs can create multiple instances of
 *             PavApp and test them with different arguments.
 *  RECEIVES - argc, cargv
 *  RETURNS  - 1 if error, 0 otherwise
 *  THROWS   - Nothing, catches any error and returns 1 on error.
 *  TODO     - Nothing
 **/

int PavApp::run( int argc, char *argv[] )
{
  vector< string > filenames;

  // name of file containing list of Archive filenames
  char* metafile = NULL;

  // PGPLOT device name
  string plot_device = "/xs";

  // Options to be set
  vector<string> options;

  // Preprocessing jobs
  vector<string> jobs;

  // verbosity
  bool verbose = false;

  bool plot_spherical = false;

  bool plot_qu = false;

  bool clear_labels = true;

  bool force_bw = false;

  vector< string > plot_ids;

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

  string clip_command = "y:range";
  string clip_value = "=(0,1)";

  char valid_args[] = "Az:hb:M:KDCdr:f:Ft:TGYSXBRmnjpP:y:H:I:N:k:ivVax:g:";

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
        jobs.push_back( string("bscrunch ") + string( optarg ) );
        break;
      }
    case 'i':
      cout << "pav VERSION $Id: PavApp.C,v 1.29 2007/11/11 22:00:55 nopeer Exp $" << endl << endl;
      return 0;
      break;
    case 'M':
      metafile = optarg;
      break;
    case 'g':
      plot_device = optarg;
      break;
    case 'D':
      plot_ids.push_back( "D" );
      break;
    case 'C':
      jobs.push_back( "centre" );
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
      plot_ids.push_back( "G" );
      break;
    case 'K':
      plot_ids.push_back( "K" );
      break;
    case 'Y':
      plot_ids.push_back( "Y" );
      break;
    case 'S':
      top_label = "pa:above:c";
      clip_command = "flux:y:range";
      options.push_back( "pa:mark=dot+tick" );
      plot_ids.push_back( "S" );
      clear_labels = false;
      break;
    case 'A':
      plot_ids.push_back( "A" );
      break;
    case 'X':
      plot_ids.push_back( "X" );
      break;
    case 'B':
      plot_ids.push_back( "B" );
      top_label = "band:above:c";
      clear_labels = false;
      break;
    case 'R':
      plot_ids.push_back( tostring<char>(c) );
      break;
    case 'm':
      plot_ids.push_back( tostring<char>(c) );
      top_label = "ell:above:c";
      //plot_spherical = true;
      break;
    case 'n':
      plot_ids.push_back( "n" );
      break;
    case 'j':
      plot_ids.push_back( "j" );
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
      force_bw = true;
      publn = true;
      options.push_back( "set=pub" );
      break;
    case PUBLNC:
      publn = true;
      options.push_back( "set=pub" );
      break;
    case 'x':
      clip_value = "=(0,";
      clip_value += string( optarg );
      clip_value += ")";
      break;
    };
  }


  // Create a list of FilePlots with the archives and a vector of plots for each

  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);


  CreatePlotsList( filenames, plot_ids );

  PavSpecificOptions();


  // If we haven't put any plots into the plot array, output an error.

  if (plots.empty() && !plot_spherical )
  {
    cout << "pav: please choose at least one plot style" << endl;
    return -1;
  }

  // set options for all the plots

  SetCmdLineOptions( options );



  // Blank out any top left corner labels

  if( clear_labels )
  {
    SetPlotOptions<FramedPlot>( "below:l=" );
  }

  // If this is not a publication plot, output a thourough title for the plot

  if( !publn )
  {
    SetPlotOptions<Plot>( top_label + string( "=$name $file. Freq: $freq MHz BW: $bw Length: $length S/N: $snr" ) );
  }
  else
  {
    SetPlotOptions<FramedPlot>( top_label + string( "=" ) );
    SetPlotOptions<FramedPlot>( "below:l=$name" );
    SetPlotOptions<FramedPlot>( "below:r=$freq MHz" );
    SetPlotOptions<StokesCylindrical>( "flux:below:l=$name" );
    SetPlotOptions<StokesCylindrical>( "flux:below:r=$freq MHz" );
    SetPlotOptions<Plot>( "ch=1.2" );
    tostring_precision = 1;
  }

  // options.push_back( clip_command + clip_value );
  SetPlotOptions<FramedPlot>( clip_command + clip_value );



  // If we still don't have any filenames
  //   output an error and exit

  if (filenames.empty())
  {
    cout << "pav: please specify filename[s]" << endl;
    return -1;
  }

  // If we can't open the pgplot device
  //   output an error and exit

  if (cpgopen(plot_device.c_str()) < 0)
  {
    cout << "pav: Could not open plot device" << endl;
    return -1;
  }

  // Determine if the device supports colour or not
  // TODO: in future, might check that it has enough colours, not just that it has colour

  if( !force_bw )
  {
    have_colour = CheckColour();
  }

  // If we received a -N option, divide the pgplot window into n1,n2 panels.

  if (n1 > 1 || n2 > 1)
    cpgsubp(n1,n2);

  // if we are plotting qu with -S, go through each of the plots and find the stokes plot
  // and set it to plot QU

  if( plot_qu )
  {
    SetPlotOptions<StokesCylindrical>( "flux:val=IQUV" );

    if( !have_colour )
    {
      SetPlotOptions<StokesCylindrical>( "flux:ci=1111" );
      SetPlotOptions<StokesCylindrical>( "flux:ls=1234" );
    }
    else
    {
      SetPlotOptions<StokesCylindrical>( "flux:ci=1264" );
      SetPlotOptions<StokesCylindrical>( "flux:ls=1111" );
    }
  }
  else
  {
    if( !have_colour )
    {
      SetPlotOptions<StokesCylindrical>( "flux:ci=111" );
      SetPlotOptions<StokesCylindrical>( "flux:ls=124" );
    }
    else
    {
      SetPlotOptions<StokesCylindrical>( "flux:ci=124" );
      SetPlotOptions<StokesCylindrical>( "flux:ls=111" );
    }
  }

  // If the plots array contains any StokesCylindrical plots, remove the label in the top left corner of the flux plot
  // SetPlotOptions<StokesCylindrical>( plots, "flux:below:l=" );

  SetPhaseZoom( min_phase, max_phase );

  if( min_freq != max_freq )
  {
    SetFreqZoom( min_freq, max_freq );
  }

  pgplot::ColourMap cmap;
  cmap.set_name ( colour_map );
  cmap.apply();


  Interpreter preprocessor;

  for (unsigned i = 0; i < plots.size(); i++)
  {
    try
    {
      if (jobs.size())
      {
        preprocessor.set( plots[i].archive );
        preprocessor.script( jobs );
      }

      if (cbppo)
      {
        Pulsar::IntegrationOrder* myio = new Pulsar::PeriastronOrder();
        plots[i].archive->add_extension(myio);
        myio->organise(plots[i].archive, ronsub);
      }

      if (cbpao)
      {
        Pulsar::IntegrationOrder* myio = new Pulsar::BinaryPhaseOrder();
        plots[i].archive->add_extension(myio);
        myio->organise(plots[i].archive, ronsub);
      }

      if (cblpo)
      {
        Pulsar::IntegrationOrder* myio = new Pulsar::BinLngPeriOrder();
        plots[i].archive->add_extension(myio);
        myio->organise(plots[i].archive, ronsub);
      }

      if (cblao)
      {
        Pulsar::IntegrationOrder* myio = new Pulsar::BinLngAscOrder();
        plots[i].archive->add_extension(myio);
        myio->organise(plots[i].archive, ronsub);
      }
      if ( plot_spherical )
      {
        spherical_wrapper( plots[i].archive );
      }
      else
      {
        // set the precision that plot will use for labels
        tostring_places = true;
        if( !publn )
          tostring_precision = 3;
        else
          tostring_precision = 1;
        for (unsigned p=0; p < plots[i].plots.size(); p++)
        {
          cpgpage ();
          plots[i].plots[p]->preprocess( plots[i].archive );
          plots[i].plots[p]->plot ( plots[i].archive );
        }
      }
    }
    catch( Error e )
    {
      cerr << "Unable to plot " << plots[i].filename << endl;
      cerr << "The following exception was encountered" << endl;
      cerr << e << endl;
    }
  }


  cpgclos();

  return 0;
}



