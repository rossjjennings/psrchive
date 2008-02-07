/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
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

#include "Phase.h"
#include "toa.h"
#include "Error.h"
#include "dirutil.h"
#include "strutil.h"

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
	"  -C \"<options>\"   Select vap-like options to be displayed on output\n"
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

  char *metafile = NULL;

  Pulsar::SmoothSinc* sinc = 0;

  string std,gaussFile;
  string outFormat("parkes"),outFormatFlags;

  vector<string> archives;
  vector<string> stdprofiles;
  vector<Tempo::toa> toas;

  Reference::To<Archive> arch;
  Reference::To<Archive> stdarch;
  Reference::To<Profile> prof;

  int gotc = 0;

  float chisq_max = 2.0;

  while ((gotc = getopt(argc, argv, "a:A:cDf:C:Fg:hiM:n:pPqS:s:tTvVx:")) != -1) {
    switch (gotc) {

    case 'a':
      std_given     = true;
      std_multiple  = true;
      std = optarg;
      if (strchr(optarg,'*')) /* have something like "*.std" */
	dirglob (&stdprofiles, optarg);
      else /* Break up inputs (e.g. have "10cm.std 20cm.std") */
	{
	  char *str;
	  str = strtok(optarg," ");
	  do 
	    {
	      stdprofiles.push_back(str);
	    } while ((str = strtok(NULL," "))!=NULL);
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
      cout << "$Id: pat.C,v 1.79 2008/02/07 01:12:38 jonathan_khoo Exp $" << endl;
      return 0;

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
      break;

   case 'q':
      Archive::set_verbosity(0);
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

    default:
      cout << "Unrecognised option " << gotc << endl;
    }
  }

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
  if (strcasecmp(outFormat.c_str(),"tempo2")==0)
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

      if (full_poln) try {
	arch->convert_state (Signal::Stokes);
	full_poln->add_observation( arch );



      }
      catch (Error& error) {
	cerr << "pat: Error MTM " << arch->get_filename() << "\n\t"
             << error.get_message() << endl;
      }
      else {
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
	    arch->toas(toas, stdarch, "", Tempo::toa::Parkes); 
	  }
	else if (strcasecmp(outFormat.c_str(),"princeton")==0)
	  arch->toas(toas, stdarch, "", Tempo::toa::Princeton); 
	else if (strcasecmp(outFormat.c_str(),"itoa")==0)
	  arch->toas(toas, stdarch, "", Tempo::toa::ITOA); 
	else if (strcasecmp(outFormat.c_str(),"psrclock")==0)
	  arch->toas(toas, stdarch, "", Tempo::toa::Psrclock); 
	else if (strcasecmp(outFormat.c_str(),"tempo2")==0)
	  {
	    string args;
	    args = archives[i];

	    if (outFormatFlags.find("i")!=string::npos) {
	      Backend* backend;
	      backend = arch->get<Backend>();
	      if (backend)
		args += string(" -i ") + backend->get_name();
	    }

	    if (outFormatFlags.find("r")!=string::npos) {
	      Receiver* receiver = arch->get<Receiver>();
	      if (receiver)
		args += string(" -r ") + receiver->get_name();
	    }

		if (outFormatFlags.find("c")!=string::npos) {
		  char temp[10];
		  sprintf(temp, "%d", arch->get_nchan());
		  args += string(" -c ") + temp;
		}

		if (outFormatFlags.find("s")!=string::npos) {
		  char temp[10];
		  sprintf(temp, "%d", arch->get_nsubint());
		  args += string(" -s ") + temp;
		}

		for (int i = 0; i < commands.size(); i++) {
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
	    arch->toas(toas, stdarch, args, Tempo::toa::Tempo2); 
	  }
      }

      for (unsigned i = 0; i < toas.size(); i++) {
		toas[i].unload(stdout);
	  }
    }
    catch (Error& error) {
      fflush(stdout); 
      cerr << error << endl;
    }
    //    if (gaussian)delete stdarch;
  }

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

string get_period( Reference::To<Archive> archive )
{
  	// TODO check this
	set_precision( 14 );
	string result = tostring<double>( archive->get_Integration(0)->get_folding_period() );
	restore_precision();

	return result;
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
		else if(command == "period") return get_period( archive );
		else if(command == "be_delay") return get_be_delay( archive );
		else if(command == "subint") return "";
		else if(command == "chan") return "";

		else return "INVALID";

	} catch (Error e) {
		return "*error*";
	}
}



