/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/psrchive.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Pulsar/ArchiveTemplates.h"
#include "Pulsar/SmoothSinc.h"

#include "Pulsar/shift_methods.h"

#include "Pulsar/PulsarCalibrator.h"

#include "Pulsar/ObsExtension.h"
#include "Pulsar/Backend.h"
#include "Pulsar/Receiver.h"
#include <Pulsar/Pointing.h>
#include <Pulsar/WidebandCorrelator.h>
#include <Pulsar/FITSHdrExtension.h>

#if HAVE_PGPLOT
#include "Pulsar/PlotFactory.h"
#include "Pulsar/Plot.h"
#include "Pulsar/ProfilePlot.h"
#include <cpgplot.h>
#endif

#include "Phase.h"
#include "toa.h"
#include "Error.h"
#include "dirutil.h"
#include "strutil.h"
#include <tostring.h>

#include <fstream>
#include <iostream>

#include <math.h>
#include <string.h>
#include <unistd.h>

using namespace std;
using namespace Pulsar;

vector<string> commands;

void loadGaussian(string file,  
		  Reference::To<Pulsar::Archive> &stdarch,  
		  Reference::To<Pulsar::Archive> arch);

string FetchValue( Reference::To< Archive > archive, string command );

double get_period(Reference::To<Archive> arch);

double get_stt_offs(Reference::To<Archive> arch);

int get_stt_smjd(Reference::To<Archive> arch);

int get_stt_imjd(Reference::To<Archive> arch);

void compute_dt(Reference::To<Archive> archive, vector<Tempo::toa>& toas,
        string std_name);

double truncateDecimals(double d, int decimalPlaces);

#if HAVE_PGPLOT
void plotDifferences(Reference::To<Archive> arch,
        Reference::To<Archive> stdarch,
        vector<Tempo::toa>& toas, const double min_phase,
        const double max_phase);

void set_phase_zoom(vector<Reference::To<Plot> >& plots,
        const double min, const double max);

void setPlotLabels(Reference::To<Archive> diff, Reference::To<Archive> arch, 
        Reference::To<Archive> profile, Pulsar::Plot* diff_plot, 
        vector<Tempo::toa>& toas, const uint subint, const uint chan);

void calculateDifference(Pulsar::Profile* diff, const float* prof,
        const float* std);

void setupPlotters(Pulsar::Plot* diff_plot, Pulsar::Plot* profile_plot,
        Pulsar::Plot* template_plot, const double min_phase,
        const double max_phase, const string filename);

void scaleProfile(Reference::To<Profile> profile);

void rotate_archive(Reference::To<Archive> archive, vector<Tempo::toa>& toas);

float getMean(const float rms, const float* bins, const uint nbin);

float getRms(const float* bins, const uint nbin, const float mean, const float oldRms);

void resize_archives(Reference::To<Archive> archive,
        Reference::To<Archive> diff, Reference::To<Archive> original);

void prepare_difference_archive(Reference::To<Archive> diff,
        Reference::To<Archive> arch, Reference::To<Profile> prof);

void difference_plot(Reference::To<Plot> plot,
        Reference::To<Archive> arch, Reference::To<Archive> diff_arch,
        vector<Tempo::toa>& toas, const uint subint, const uint chan);

void template_plot(Reference::To<Plot> plot, Reference::To<Archive> arch);

void profile_plot(Reference::To<Plot> plot,
        Reference::To<Archive> profile_archive,
        Reference::To<Profile> profile_to_copy, Reference::To<Archive> archive,
        const double freq);

void diff_profiles(Reference::To<Archive> diff, Reference::To<Archive> stdarch, Reference::To<Profile> profile);

string get_xrange(const double min, const double max);
#endif // HAVE_PGPLOT

////////////////////////////////////////////////////////////////////////////////////////////
// PRECISION FOR tostring
////////////////////////////////////////////////////////////////////////////////////////////


unsigned int old_precision;
bool old_places;

void set_precision( unsigned int num_digits, unsigned int places = true )
{
  old_precision = tostring_precision;
  old_places = tostring_places;

  tostring_precision = num_digits;
  tostring_places = places;
}


void restore_precision( void )
{
  tostring_precision = old_precision;
  tostring_places = old_places;
}

void usage ()
{
  cout << "pat - Pulsar::Archive timing \n"
    "Usage: pat [options] filenames \n"
    "  -q               Quiet mode \n"
    "  -v               Verbose mode \n"
    "  -V               Very verbose mode \n"
    "  -i               Show revision information \n"
    "  -M metafile      List of archive filenames in metafile \n"
    "\n"
    "Preprocessing options:\n"
    "  -F               Frequency scrunch before fitting \n"
    "  -T               Time scrunch before fitting \n"
    "  -d               Discard profiles with zero weight\n"
    "\n"
    "Fitting options:\n"
    "  -a stdfiles      Automatically select standard from specified group\n"
    "  -D               Denoise standard \n"
    "  -g datafile      Gaussian model fitting \n"
    "  -s stdfile       Location of standard profile \n"
    "  -S period        Zap harmonics due to periodic spikes in profile \n"
    "                   (use of this option implies SIS) \n"
    "\n"
    "Matrix template matching options: \n"
    "  -c               Choose the maximum harmonic \n"
    "  -n harmonics     Use up to the specified number of harmonics\n"
    "  -p               Enable matrix template matching \n"
    "  -P               Do not fscrunch the standard \n"
    "\n"
    "Algorithm Selection:\n"
    "  -A name          Select shift algorithm [default: PGS] \n"
    "                   PGS = Fourier phase gradient \n"
    "                   GIS = Gaussian interpolation \n"
    "                   PIS = Parabolic interpolation \n"
    "                   ZPF = Zero pad interpolation \n"
    "                   SIS = Sinc interpolation of cross-corration function\n"
    "\n"
    "Output options:\n"
    "  -f \"fmt <flags>\" Select output format [default: parkes]\n"
    "                   Available formats: parkes tempo2, itoa, princeton \n"
    "                   For tempo2, <flags> include i = display instrument \n"
    "                                               r = display receiver   \n"
	"  -C \"<options>\"   Select vap-like options to be displayed on output \n"
    "  -r               Print reference phase and dt \n"
    "  -u               Print as pat-like format smjd + dt \n"
    "\n"
    "Plotting options (if compiled with pgplot):\n"
    "  -K               Specify plot device\n"
    "  -t               Plot template, profile and difference \n"
    "\n"
    "See "PSRCHIVE_HTTP"/manuals/pat for more details\n"
       << endl;
}


int main (int argc, char *argv[]) try {
  
  bool verbose = false;
  bool std_given = false;
  bool std_multiple = false;
  bool gaussian = false;
  bool full_freq = false;

  // the reception calibration class
  Pulsar::PulsarCalibrator* full_poln = 0;
  bool choose_maximum_harmonic = false;
  unsigned maximum_harmonic = 0;

  bool fscrunch = false;
  bool tscrunch = false;
  bool skip_bad = false;
  bool phase_info = false;
  bool tempo2_output = false;

#if HAVE_PGPLOT
  bool plot_difference = false;
#endif

  char *metafile = NULL;

  Pulsar::SmoothSinc* sinc = 0;

  string std,gaussFile;
  string outFormat("parkes"),outFormatFlags;
  string tname;

  double min_phase = 0.0;
  double max_phase = 1.0;
  string plot_device = "/xs";

  vector<string> archives;
  vector<string> stdprofiles;
  vector<Tempo::toa> toas;

  Reference::To<Archive> arch;
  Reference::To<Archive> stdarch;
  Reference::To<Profile> prof;

  float chisq_max = 2.0;

#if HAVE_PGPLOT
  const char* args = "a:A:cDdf:C:Fg:hiK:M:n:pPqrS:s:tTuvVx:z:";
#else
  const char* args = "a:A:cDdf:C:Fg:hiK:M:n:pPqrS:s:TuvVx:z:";
#endif

  int gotc = 0;

  while ((gotc = getopt(argc, argv, args)) != -1)
  {
    switch (gotc)
    {

    case 'a':
      std_given     = true;
      std_multiple  = true;
      std = optarg;

      /* Break up inputs (e.g. have "10cm.std 20cm.std 50*.std") */
      {
	char *str;
	str = strtok(optarg," ");
	do 
	{
	  dirglob (&stdprofiles, str);
	}
	while ((str = strtok(NULL," "))!=NULL);
      }

      if (stdprofiles.size() == 0)
      {
	cerr << "pat -a \"" << optarg << "\" failed: no such file" << endl;
	return -1;
      }
      break;

    case 'A':

      if (strcasecmp (optarg, "PGS") == 0)
	Profile::shift_strategy.set(&PhaseGradShift);
      
      else if (strcasecmp (optarg, "GIS") == 0)
	Profile::shift_strategy.set(&GaussianShift);
      
      else if (strcasecmp (optarg, "PIS") == 0)
	Profile::shift_strategy.set(&ParIntShift);

      else if (strcasecmp (optarg, "ZPS") == 0)
	Profile::shift_strategy.set(&ZeroPadShift);
      
      else if (strcasecmp (optarg, "SIS") == 0)
	Profile::shift_strategy.set(&SincInterpShift);

      else if (strcasecmp (optarg, "FDM") == 0)
	Profile::shift_strategy.set(&FourierDomainFit);
      
      else
	cerr << "pat: unrecognized shift method '" << optarg << "'" << endl;

      break;

    case 'c':
      choose_maximum_harmonic = true;
      break;

	case 'C':
		separate(optarg, commands, " ,");
		break;

    case 'D':
      sinc = new Pulsar::SmoothSinc;
      sinc -> set_bins (8);
      break;

    case 'd':
      skip_bad = true;
      break;

    case 'F':
      fscrunch = true;
      break;

    case 'f':
      {
	/* Set the output format */
	outFormat = string(optarg).substr(0,string(optarg).find(" "));
	if (string(optarg).find(" ")!=string::npos)
	  outFormatFlags = string(optarg).substr(string(optarg).find(" "));
	break;
      }

    case 'g':
      gaussian  = true;
      gaussFile = optarg;
      break;

    case 'h':
      usage ();
      return 0;

    case 'i':
      cout << "$Id: pat.C,v 1.85 2009/06/15 06:12:13 jonathan_khoo Exp $" << endl;
      return 0;

    case 'K':
      plot_device = optarg;
      break;

    case 'M':
      metafile = optarg;
      break;

    case 'n':
      maximum_harmonic = atoi(optarg);
      break;

    case 'P':
      full_freq = true;
      break;

    case 'p':
      full_poln = new Pulsar::PulsarCalibrator;
      full_poln->set_solve_each ();
      break;

    case 'q':
      Archive::set_verbosity(0);
      break;

    case 'r':
      phase_info = true;
      outFormat = "tempo2";
      break;

    case 'S':
      Profile::shift_strategy.set(&SincInterpShift);      
      Pulsar::SIS_zap_period = atoi(optarg);
      break;

    case 's':
      std_given = true;
      std = optarg;
      break;

    case 'T':
      tscrunch = true;
      break;

#if HAVE_PGPLOT
    case 't':
      plot_difference = true;
      break;
#endif

    case 'u':
      tempo2_output = true;
      outFormat = "tempo2";
      break;

    case 'v':
      Archive::set_verbosity(2);
      PulsarCalibrator::verbose = 2;
      verbose = true;
      break;

    case 'V':
      Archive::set_verbosity(3);
      PulsarCalibrator::verbose = 3;
      verbose = true;
      break;

    case 'x':
      chisq_max = atof(optarg);
      if (chisq_max == 0.0)
        cerr << "pat: disabling reduced chisq cutoff" << endl;
      else
        cerr << "pat: omitting TOAs with reduced chisq > " << chisq_max << endl;
      break;

    case 'z':
      {
        string s1, s2;
        string_split(optarg, s1, s2, ",");
        min_phase = fromstring<double>(s1);
        max_phase = fromstring<double>(s2);
      }
      break;

    default:
      cout << "Unrecognised option " << gotc << endl;
    }
  }

#if HAVE_PGPLOT
  if (plot_difference)
      if (cpgopen(plot_device.c_str()) <= 0)
          cpgopen("?");
#endif

  if (metafile)
    stringfload (&archives, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&archives, argv[ai]);
  
  if (archives.empty()) {
    cerr << "No archives were specified" << endl;
    return -1;
  } 
  
  if (!std_given && !gaussian) {
    cerr << "You must specify the standard profile to use!" << endl;
    return -1;
  }

  if (!std_multiple && !gaussian) try {

    // If only using one standard profile ...

    stdarch = Archive::load(std);

    if (!full_freq)
		stdarch->fscrunch();

    stdarch->tscrunch();
    
    if (sinc)
      Pulsar::foreach (stdarch, sinc);
    
    if (full_poln) {
      
      cerr << "pat: using full polarization" << endl;

      if (maximum_harmonic)
	full_poln->set_maximum_harmonic (maximum_harmonic);

      full_poln->set_choose_maximum_harmonic (choose_maximum_harmonic);

      full_poln->set_tim_file (stdout);

      if (strcasecmp(outFormat.c_str(),"parkes")==0) 
        full_poln->set_toa_format(Tempo::toa::Parkes);
      else if (strcasecmp(outFormat.c_str(),"princeton")==0)
        full_poln->set_toa_format(Tempo::toa::Princeton);
      else if (strcasecmp(outFormat.c_str(),"itoa")==0)
        full_poln->set_toa_format(Tempo::toa::ITOA);
      else if (strcasecmp(outFormat.c_str(),"psrclock")==0)
        full_poln->set_toa_format(Tempo::toa::Psrclock);
      else if (strcasecmp(outFormat.c_str(),"tempo2")==0)
        full_poln->set_toa_format(Tempo::toa::Tempo2);

      stdarch->convert_state (Signal::Stokes);
      full_poln->set_standard( stdarch );

      cerr << "pat: last harmonic = " << full_poln->get_nharmonic() << endl;

    }
    else
      stdarch->pscrunch();

  }
  catch (Error& error) {
    cerr << "Error processing standard profile:" << endl;
    cerr << error << endl;
    return -1;
  }

  // Give format information for Tempo2 output 

  if (strcasecmp(outFormat.c_str(),"tempo2")==0 && !phase_info)
#if HAVE_PGPLOT
      if (!plot_difference)
#endif
          cout << "FORMAT 1" << endl;

  for (unsigned i = 0; i < archives.size(); i++) {
    
    try {

      if (verbose)
	cerr << "Loading " << archives[i] << endl;
      
      arch = Archive::load(archives[i]);
      if (i==0 && gaussian)
	loadGaussian(gaussFile, stdarch, arch);
      if (fscrunch)
	arch->fscrunch();
      if (tscrunch)
	arch->tscrunch();

	if (full_freq) {
	  if (stdarch->get_nchan() < arch->get_nchan()) {
		  arch->fscrunch(arch->get_nchan() / stdarch->get_nchan());
	  } else if (stdarch->get_nchan() > arch->get_nchan()) {
		  stdarch->fscrunch(stdarch->get_nchan() / arch->get_nchan());
	  } 
	}

      if (full_poln) try
      {
	arch->convert_state (Signal::Stokes);
	full_poln->add_observation( arch );
      }
      catch (Error& error)
      {
	cerr << "pat: Error MTM " << arch->get_filename() << error << endl;
      }
      else
      {
	arch->convert_state (Signal::Intensity);

	/* If multiple standard profiles given must now choose and load 
           the one closest in frequency */
	if (std_multiple) {	  
	  double freq = arch->get_centre_frequency();
	  double minDiff=0.0;
	  int    jDiff=0;
	  unsigned j;
	  for (j = 0;j < stdprofiles.size();j++)	    
	    {
	      stdarch = Archive::load(stdprofiles[j]);	      
	      if (j==0 || fabs(stdarch->get_centre_frequency() - freq)<minDiff)
		{
		  minDiff = fabs(stdarch->get_centre_frequency()-freq);
		  jDiff   = j;
		}
	    }
	  stdarch = Archive::load(stdprofiles[jDiff]);
	  stdarch->fscrunch();
	  stdarch->tscrunch();
	
          if (sinc)
            Pulsar::foreach (stdarch, sinc);
	
	  stdarch->convert_state(Signal::Intensity);
	}
	if (strcasecmp(outFormat.c_str(),"parkes")==0)
	  {
	    arch->toas(toas, stdarch, "", Tempo::toa::Parkes, skip_bad); 
	  }
	else if (strcasecmp(outFormat.c_str(),"princeton")==0)
	  arch->toas(toas, stdarch, "", Tempo::toa::Princeton, skip_bad); 
	else if (strcasecmp(outFormat.c_str(),"itoa")==0)
	  arch->toas(toas, stdarch, "", Tempo::toa::ITOA, skip_bad); 
	else if (strcasecmp(outFormat.c_str(),"psrclock")==0)
	  arch->toas(toas, stdarch, "", Tempo::toa::Psrclock, skip_bad); 
	else if (strcasecmp(outFormat.c_str(),"tempo2")==0)
	  {
	    string args;
	    args = archives[i];

	    if (outFormatFlags.find("i")!=string::npos)
	    {
	      Backend* backend;
	      backend = arch->get<Backend>();
	      if (backend)
		args += " -i " + backend->get_name();
	    }

	    if (outFormatFlags.find("r")!=string::npos)
	    {
	      Receiver* receiver = arch->get<Receiver>();
	      if (receiver)
		args += " -r " + receiver->get_name();
	    }

	    if (outFormatFlags.find("c")!=string::npos)
	      args += " -c " + tostring(arch->get_nchan());

	    if (outFormatFlags.find("s")!=string::npos)
	      args += " -s " + tostring(arch->get_nsubint());

	    for (unsigned i = 0; i < commands.size(); i++)
	    {
	      string value = FetchValue(arch, commands[i]);
	      args += " -" + commands[i] + " " + value;
	    }


	    if (outFormatFlags.find("o")!=string::npos) /* Include observer info. */
	      {
		const ObsExtension* ext = 0;
		ext = arch->get<ObsExtension>();
		if (!ext) {
		  args += " -o N/A";
		}
		else {
		  args += " -o " + ext->observer;
		}

	      }

	    arch->toas(toas, stdarch, args, Tempo::toa::Tempo2, skip_bad); 
	  }
      }

#if HAVE_PGPLOT
      if (plot_difference) {
          rotate_archive(arch, toas);
          arch->remove_baseline();
          plotDifferences(arch, stdarch, toas, min_phase, max_phase);
          continue;
      }
#endif

      if (phase_info) {
          compute_dt(arch, toas, std);
      } else {
          if (tempo2_output) {
              vector<Tempo::toa>::iterator tit;
              for (tit = toas.begin(); tit != toas.end(); ++tit)
                  (*tit).set_phase_info(true);
          }

          for (unsigned i = 0; i < toas.size(); i++)
              toas[i].unload(stdout);
      }
    }
    catch (Error& error) {
      fflush(stdout); 
      cerr << error << endl;
    }
    //    if (gaussian)delete stdarch;
  }

#if HAVE_PGPLOT
  if (plot_difference)
      cpgend();
#endif

  fflush(stdout);
  return 0;

}
catch (Error& error) {
  cerr << error << endl;
  return -1;
}


void loadGaussian(string file,  Reference::To<Archive> &stdarch,  Reference::To<Archive> arch)
{
  ifstream fin(file.c_str());
  string psr,str;
  char   buf[1000];
  float  tau,freq,spos,swidth,samp;
  static vector<float> pos,width,amp;
  static unsigned int    ncomp;
  float version;
  unsigned n=0;
  static bool firstTime=true;

  if (firstTime)
    {
      while (!fin.eof())
	{
	  fin >> str;
	  if (str.at(0)=='#') fin.getline(buf,1000);
	  else if (str.compare("Version:")==0) fin >> version;
	  else if (str.compare("Pulsar:")==0)  fin >> psr;
	  else if (str.compare("Freq")==0)     fin >> str >> freq;
	  else if (str.compare("Tau")==0)      fin >> str >> tau;
	  else if (str.compare("Components:")==0) fin >> ncomp;
	  else
	    {
	      fin >> spos >> samp >> swidth;  n++;
	      pos.push_back(spos);
	      amp.push_back(samp);
	      width.push_back(swidth);
	    }
	}
      if (n != ncomp)
	{
	  cerr << "Warning: number of Gaussian components " << 
	    ncomp << " does not equal components provided " << n << endl;
	  ncomp = n;
	}
    }
 
  stdarch = arch->clone();
  stdarch->fscrunch();
  stdarch->tscrunch();
  stdarch->pscrunch();
  
  // Now replace profile with Gaussian components
  Reference::To<Profile> prof;
  float *amps;
  double x,y;
  unsigned int i,j;

  prof = stdarch->get_Profile(0,0,0);
  amps = prof->get_amps();
  for (i=0;i<stdarch->get_nbin();i++)
    {
      x = double(i)*360.0/double(stdarch->get_nbin());
      y = 0.0;
      double two = 2.0;
      for (j=0;j<ncomp;j++)
	y+=amp[j]*exp(-pow(x-pos[j],two)/pow(double(width[j]),two));
      amps[i]=y;
      if (firstTime) cout << i << " " << y << " PROFILE " << endl;
    }
  prof->set_amps(amps);
  firstTime=false;
}

string get_name( Reference::To< Archive > archive )
{
	return archive->get_source();
}

string get_nbin( Reference::To< Archive > archive )
{
	return tostring( archive->get_nbin() );
}

string get_nchan( Reference::To< Archive > archive )
{
	return tostring( archive->get_nchan() );
}

string get_npol( Reference::To< Archive > archive )
{
	return tostring( archive->get_npol() );
}

string get_nsub( Reference::To< Archive > archive )
{
	return tostring( archive->get_nsubint() );
}

string get_length( Reference::To< Archive > archive )
{
  tostring_precision = 6;
  return tostring( archive->integration_length() );
}

string get_bw( Reference::To< Archive > archive )
{
  	set_precision(3);
  	string result = tostring(archive->get_bandwidth());
  	restore_precision();
  	return result;
}

string get_parang( Reference::To< Archive > archive )
{
  	stringstream result;
  	int nsubs = archive->get_nsubint();

  	if (nsubs != 0) {
		Reference::To< Integration > integration = archive->get_Integration( nsubs / 2 );

    	if (integration) {
      		Reference::To< Pointing > ext = integration->get<Pointing>();

      		if (ext) {
        		result << ext->get_parallactic_angle().getDegrees();
      		}
    	}
  	}

  	return result.str();
}

string get_tsub( Reference::To< Archive > archive )
{
  	string result;
  	int nsubs = archive->get_nsubint();

  	if( nsubs != 0 ) {
    	Reference::To< Integration > first_int = archive->get_first_Integration();

    		if( first_int ) {
      			set_precision( 6 );
      			result = tostring( first_int->get_duration() );
      			restore_precision();
    		}
  	}

  	return result;
}

string get_observer( Reference::To<Archive> archive )
{
  	string result;
  	Reference::To< ObsExtension > ext = archive->get<ObsExtension>();

  	if( !ext ) {
    	result = "UNDEF";

  	} else {
    	result = ext->get_observer();
  	}

  	return result;
}

string get_projid( Reference::To<Archive> archive )
{
  	string result = "";
  	Reference::To<ObsExtension> ext = archive->get<ObsExtension>();

  	if( !ext ) {
    	result = "UNDEF";

  	} else {
    	result = ext->get_project_ID();
  	}

  	return result;
}

string get_rcvr( Reference::To<Archive> archive )
{
  	string result;
  	Reference::To<Receiver> ext = archive->get<Receiver>();

  	if( ext ) {
    	result = ext->get_name();
  	}

  	return result;
}

string get_backend( Reference::To< Archive > archive )
{
  	string result = "";
  	Reference::To<Backend> ext;
  	ext = archive->get<Backend>();

  	if( !ext ) {
    	ext = archive->get<WidebandCorrelator>();
  	}

  	if( !ext ) {
    	result = "UNDEF";
	} else {
    	result = ext->get_name();
	}

  	return result;
}

string get_period_as_string( Reference::To<Archive> archive )
{
  	// TODO check this
	set_precision( 14 );
	string result = tostring<double>( archive->get_Integration(0)->get_folding_period() );
	restore_precision();

	return result;
}

double get_period(Reference::To<Archive> arch)
{
    return arch->get_Integration(0)->get_folding_period();
}

double get_stt_offs(Reference::To<Archive> arch)
{
    return arch->get<FITSHdrExtension>()->get_stt_offs();
}

int get_stt_smjd(Reference::To<Archive> arch)
{
    return arch->get<FITSHdrExtension>()->get_stt_smjd();
}

int get_stt_imjd(Reference::To<Archive> arch)
{
    return arch->get<FITSHdrExtension>()->get_stt_imjd();
}

string get_be_delay( Reference::To< Archive > archive )
{
	string result;
	Reference::To<Backend> ext = archive->get<WidebandCorrelator>();
	set_precision( 14 );

	if( !ext ) {
		result = "UNDEF";

	} else {
		result = tostring<double>( ext->get_delay() );
	}

	restore_precision();
	return result;
}

string FetchValue (Reference::To<Archive> archive, string command)
{
	try {

		if (command == "name") return get_name( archive );
		else if(command == "nbin") return get_nbin( archive );
		else if(command == "nchan") return get_nchan( archive );
		else if(command == "npol") return get_npol( archive );
		else if(command == "nsub") return get_nsub( archive );
		else if(command == "length") return get_length( archive );
		else if(command == "bw") return get_bw( archive );
		else if(command == "parang") return get_parang( archive );
		else if(command == "tsub") return get_tsub( archive );
		else if(command == "observer") return get_observer( archive );
		else if(command == "projid") return get_projid( archive );
		else if(command == "rcvr") return get_rcvr( archive );
		else if(command == "backend") return get_backend( archive );
		else if(command == "period") return get_period_as_string( archive );
		else if(command == "be_delay") return get_be_delay( archive );
		else if(command == "subint") return "";
		else if(command == "chan") return "";

		else return "INVALID";

	} catch (Error e) {
		return "*error*";
	}
}

/**
 * @brief Plot three profiles on one page:
 *        - difference between the template and profile
 *        - template
 *        - archive for each subint and channel
 *
 * @param arch Target archive whose profiles will be subtracted from the
 *             template.
 * @param stdarch Template archive.
 * @param min_phase min x-value when zooming
 * @param max_phase max x-value when zooming
 */

#if HAVE_PGPLOT
void plotDifferences(Reference::To<Archive> arch,
        Reference::To<Archive> stdarch, vector<Tempo::toa>& toas,
        const double min_phase, const double max_phase)
{
    // remove baseline for all templates (except caldelay)
    const bool cal_delay_file = arch->get_source() == "CalDelay";
    if (!cal_delay_file)
        stdarch->remove_baseline();

    // difference between template and profile
    Reference::To<Archive> profile_diff = Archive::new_Archive("PSRFITS");

    // current profile
    Reference::To<Archive> profile_archive = Archive::new_Archive("PSRFITS");
    resize_archives(profile_archive, profile_diff, arch);

    Pulsar::PlotFactory factory;
    Reference::To<Plot> plotter = factory.construct("flux");

    // adjust x-range if zoom has been specified
    if (min_phase != 0.0 || max_phase != 1.0)
        plotter->configure("x:range=" + get_xrange(min_phase, max_phase));

    cpgsubp(1, 3);
    for (uint isub = 0; isub < arch->get_nsubint(); ++isub) {
        for (uint ichan = 0; ichan < arch->get_nchan(); ++ichan) {
            cpgpage();
            Pulsar::Profile* profile = arch->get_Profile(isub, 0, ichan);
            if (cal_delay_file)
                scaleProfile(profile);

            double freq = profile->get_centre_frequency();
            freq = truncateDecimals(freq, 3);

            diff_profiles(profile_diff, stdarch, profile);
            difference_plot(plotter, arch, profile_diff, toas, isub, ichan);
            template_plot(plotter, stdarch);
            profile_plot(plotter, profile_archive, profile, arch, freq);
        }
    }
    cout << "Plotting " << arch->get_filename() << endl;
}
#endif

/**
 * @brief compute delta time using the phase shift and start time offset
 * @param archive archive where the TOAs were calculated
 * @param toas vector of TOAs for each subint and chan
 * @param std_name filename of the standard template
 */

void compute_dt(Reference::To<Archive> archive, vector<Tempo::toa>& toas,
        string std_name)
{
    const double period = get_period(archive);
    const double stt_offs = get_stt_offs(archive);

    vector<Tempo::toa>::iterator tit;
    for (tit = toas.begin(); tit != toas.end(); ++tit) {
        const double phaseShift = (*tit).get_phase_shift();
        double dt = phaseShift * period;

        dt += stt_offs;
        dt *= 1000.0; // ms
        dt = fmod(dt + 10.0, 1.0); // to allow for dt < 1.0

        if (dt > 0.5)
            dt -= 1.0;

        dt *= 1000.0;  // microsec

        // remove preceeding path to shorten output line
        uint pos = std_name.find_last_of('/');
        if (pos != string::npos)
            std_name = std_name.substr(pos + 1, std_name.length() - pos);

        cout << fixed << archive->get_filename() << " " <<  std_name << " " <<
            (*tit).get_subint() << " " << (*tit).get_channel() << " ";

        cout << setprecision(9) << stt_offs << " "; 
        cout << setprecision(7) << phaseShift << " "; 

        cout << setprecision(3) << dt << " " << setprecision(3) <<
            (*tit).get_error() << endl;
    }
}


/**
 * @param min min x-value
 * @param max max x-value
 * @throws InvalidRange if input min >= max
 * @returns the x:range string command to implement zoom 
 */

string get_xrange(const double min, const double max)
{
    if (min >= max)
        throw Error(InvalidRange, "set_phase_zoom", "min (%g) >= max (%g)",
                min, max);

    return string("(") +
        tostring<double>(min) +
        string(")") +
        tostring<double>(max) +
        string( ")");
}


/**
 * @brief resize the new archives (for plotting) to match the original
 *        archive
 */

void resize_archives(Reference::To<Archive> archive,
        Reference::To<Archive> diff, Reference::To<Archive> original)
{
    const uint nsub = original->get_nsubint();
    const uint nchan = original->get_nchan();
    const uint npol = original->get_npol();
    const uint nbin = original->get_nbin();

    diff->resize(nsub, npol, nchan, nbin);
    archive->resize(nsub, npol, nchan, nbin);
}


/**
 * @brief rotate each profile in the archive by the corresponding phase shift
 * @param archive archive whose profiles will be rotated
 * @param toas vector of TOAs for each subint and chan
 */

void rotate_archive(Reference::To<Archive> archive, vector<Tempo::toa>& toas)
{
    const uint nchan = archive->get_nchan();
    const uint nsub = archive->get_nsubint();

    vector<Tempo::toa>::iterator it = toas.begin();
    for (uint isub = 0; isub < nsub; ++isub) {
        for (uint ichan = 0; ichan < nchan; ++ichan, ++it) {
            const double phase_shift = (*it).get_phase_shift();
            archive->get_Profile(isub, 0, ichan)->rotate_phase(phase_shift);
        }
    }
}


/**
 * @brief copy the frequency (3dp) and source name from one archive to another
 * @param prof profile of the current plotted profile
 */

void prepare_difference_archive(Reference::To<Archive> diff,
        Reference::To<Archive> arch, Reference::To<Profile> prof)
{
    const double freq = truncateDecimals(prof->get_centre_frequency(), 3);
    diff->set_source(arch->get_source());
    diff->set_centre_frequency(freq);
}

/**
 * @brief set up and plot the difference profile
 */

#if HAVE_PGPLOT
void difference_plot(Reference::To<Plot> plot,
        Reference::To<Archive> arch, Reference::To<Archive> diff_arch,
        vector<Tempo::toa>& toas, const uint subint, const uint chan)
{
    Reference::To<Profile> profile = arch->get_Profile(subint, 0, chan);
    prepare_difference_archive(diff_arch, arch, profile);

    const uint i = subint * arch->get_nchan() + chan;
    const double snr = diff_arch->get_Profile(0,0,0)->snr();

    char phaseShift[50];
    char phaseError[50];
    sprintf(phaseShift, "%.4f", toas[i].get_phase_shift());
    sprintf(phaseError, "%.4g", toas[i].get_error());

    // set the heading to read:
    // filename, subint, channel, snr, phase shift, phase error
    plot->configure("above:c=File: " + arch->get_filename() +
            "\nSubint: " + tostring(subint) + " Chan: " + tostring(chan) +
            " S/N: " + tostring(snr) + " Phase Shift: " + phaseShift +
            " Phase Error: " + phaseError);

    plot->configure("ch=2");
    plot->plot(diff_arch);
    cpgpage();
}


/**
 * @brief set up and plot the standard template
 */

void template_plot(Reference::To<Plot> plot, Reference::To<Archive> arch)
{
    plot->configure("above:c=Template: " + arch->get_filename());
    plot->plot(arch);
    cpgpage();
}

void profile_plot(Reference::To<Plot> plot,
        Reference::To<Archive> profile_archive,
        Reference::To<Profile> profile_to_copy, Reference::To<Archive> archive,
        const double freq)
{
    *(profile_archive->get_Profile(0,0,0)) = *profile_to_copy;

    profile_archive->set_source(archive->get_source());
    profile_archive->set_centre_frequency(freq);

    plot->configure("above:c=Profile");
    plot->plot(profile_archive);
}

/**
 * @brief calculate the difference between the template and current profile
 *        and store it as a separate profile - 'diff'
 */

void diff_profiles(Reference::To<Archive> diff, Reference::To<Archive> stdarch, Reference::To<Profile> profile)
{
    Reference::To<Profile> std_profile = stdarch->get_Profile(0,0,0);
    const double stdarch_area = std_profile->sum();
    const double profile_area = profile->sum();
    const double scale = stdarch_area / profile_area;
    profile->scale(fabs(scale));

    *(diff->get_Profile(0,0,0)) = *std_profile;
    diff->get_Profile(0,0,0)->diff(profile);
}

void scaleProfile(Reference::To<Profile> profile)
{
    float* bins = profile->get_amps();
    const uint nbin = profile->get_nbin();

    // if baseline removal is not effective, subtract the mean from the profile
    double stats_mean = profile->mean(1.0);

    if (profile->min(0, profile->get_nbin()) > 0.0)
        *profile -= stats_mean;

    // calculate mean and rms from all points < 3 * rms
    // subtract mean from all points
    // recompute mean and rms of all points < 3 * rms
    // repeat until delta_mean < 0.01*RMS

    float mean = 0.0;
    float deviation = 0.0;
    float oldMean = 0.0;

    while (true) {
        mean = getMean(deviation, bins, nbin);
        deviation = getRms(bins, nbin, mean, deviation);

        if (fabs(mean - oldMean) < 0.01 * fabs(deviation)) {
            *profile -= mean;
            break;
        }

        *profile -= mean;
        oldMean = mean;
    }
}


/**
 * @brief calculate new mean for all points < 3(rms)
 */

float getMean(const float rms, const float* bins, const uint nbin)
{
    float mean = 0.0;
    int count = 0;

    // calculate mean
    for (uint i = 0; i < nbin; ++i) {
        if (bins[i] < 3.0 * rms) {
            mean += bins[i];
            ++count;
        }
    }

    mean /= (float)count;
    return mean;
}


/**
 * @brief calculate new rms for all points < 3(rms)
 */

float getRms(const float* bins, const uint nbin, const float mean,
        const float oldRms)
{
    float deviation = 0.0;
    int count = 0;

    for (uint i = 0; i < nbin; ++i) {
        if (bins[i] < 3.0 * oldRms) {
            deviation += pow((bins[i] - mean), 2); // square
            ++count;
        }
    }

    deviation /= (float)count;
    return sqrt(deviation);
}
#endif // if HAVE_PGPLOT


/**
 * @brief a (stupid?) way to remove decimal places from a number
 * @param d floating-point number to be truncated
 * @param decimalPlaces number of decimal places to truncate to
 * @returns a truncated floating-point number
 */

double truncateDecimals(double d, int decimalPlaces)
{
    double result = d * pow(10, (double)decimalPlaces);
    result = (int)result;
    result /= pow(10, (double)decimalPlaces);

    return result;
}
