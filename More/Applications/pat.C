 
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Plotter.h"
#include "Pulsar/Plotter.h"
#include "Pulsar/Profile.h"
#include "Pulsar/getopt.h"

#include "Pulsar/PolnProfile.h"
#include "Pulsar/PolnProfileFit.h"
#include "Calibration/Polar.h"

#include "Pulsar/ObsExtension.h"

#include "Phase.h"
#include "toa.h"
#include "Error.h"
#include "dirutil.h"
#include "genutil.h"

#include <cpgplot.h>
#include <math.h>
#include <string.h>

#include <fstream.h>
#include <iostream.h>


void loadGaussian(string file,  Reference::To<Pulsar::Archive> &stdarch,  Reference::To<Pulsar::Archive> arch);


void usage ()
{
  cout << "pat - Pulsar::Archive timing \n"
    "Usage: pat [options] filenames \n"
    "  -v               Verbose mode \n" 
    "  -V               Very verbose mode \n"
    "  -i               Show revision information \n"
    "\n"
    "Preprocessing options:\n"
    "  -F               Frequency scrunch before fitting \n"
    "  -T               Time scrunch before fitting \n"
    "\n"
    "Fitting options:\n"
    "  -a stdfiles      Automatic selection of standard profiles from frequency \n"
    "  -g datafile      Gaussian model fitting \n"
    "  -n harmonics     Use up to the specified number of harmonics\n"
    "  -p               Perform full polarimetric fit in Fourier domain \n"
    "  -s stdfile       Location of standard profile \n"
    "  -t               Fit in the time domain \n"
    "  -D               Denoise standard \n"
    "\n"
    "Output options:\n"
    "  -f \"format <flags>\"  Output format (parkes = default, tempo2, itoa, princeton ...)\n"
    "                       For tempo2 output the flags are i = display instrument \n"
    "                                                       r = display receiver   \n"
    "See http://astronomy.swin.edu.au/pulsar/software/manuals/pat.html"
       << endl;
}

int main (int argc, char *argv[])
{
  
  bool verbose = false;
  bool std_given = false;
  bool std_multiple = false;
  bool gaussian = false;
  bool time_domain = false;
  bool full_poln = false;

  bool fscrunch = false;
  bool tscrunch = false;

  bool denoise = false;
  string std,gaussFile;
  string outFormat("parkes"),outFormatFlags;

  vector<string> archives;
  vector<string> stdprofiles;
  vector<Tempo::toa> toas;

  int gotc = 0;

  Pulsar::PolnProfileFit fit;
  while ((gotc = getopt(argc, argv, "hiDFn:ps:g:a:tTvV:f:")) != -1) {
    switch (gotc) {
    case 'h':
      usage ();
      return 0;

    case 'v':
      verbose = true;
      break;

    case 'V':
      verbose = true;
      Pulsar::Archive::set_verbosity(3);
      Calibration::Model::verbose = true;
      break;
    case 'D':
      denoise = true;
      break;
    case 'i':
      cout << "$Id: pat.C,v 1.26 2004/06/08 01:12:15 sord Exp $" << endl;
      return 0;

    case 'F':
      fscrunch = true;
      break;

    case 'n':
      fit.set_maximum_harmonic( atoi(optarg) );
      break;

    case 'T':
      tscrunch = true;
      break;

    case 'g':
      gaussian  = true;
      gaussFile = optarg;
      break;

    case 'p':
      full_poln = true;
      break;

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

    case 's':
      std_given = true;
      std = optarg;
      break;

    case 't':
      time_domain = true;
      break;

    case 'f':
      {
	outFormat = string(optarg).substr(0,string(optarg).find(" "));  /* Set the output format */
	if (string(optarg).find(" ")!=string::npos) outFormatFlags = string(optarg).substr(string(optarg).find(" "));
	break;
      }
    default:
      cout << "Unrecognised option " << gotc << endl;
    }
  }
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

  Reference::To<Pulsar::Archive> arch;
  Reference::To<Pulsar::Archive> stdarch;
  Reference::To<Pulsar::Profile> prof;

  Reference::To<const Pulsar::PolnProfile> poln_profile;

  if (!std_multiple && !gaussian) /* If only using one standard profile */
    {
      try {
	stdarch = Pulsar::Archive::load(std);
	stdarch->fscrunch();
	stdarch->tscrunch();
	
        if (denoise)
          stdarch->denoise();
	
	if (full_poln) {
	  
	  cerr << "pat: full polarization fitting with " << std << endl;
	  fit.set_standard( stdarch->get_Integration(0)->new_PolnProfile(0) );
	  fit.set_transformation( new Calibration::Polar );
	  
	}
	else
	  stdarch->convert_state(Signal::Intensity);
	
      }
      catch (Error& error) {
	cerr << "Error processing standard profile:" << endl;
	cerr << error << endl;
	return -1;
      }
    }
  // Give format information for Tempo2 output 
  if (strcasecmp(outFormat.c_str(),"tempo2")==0)
    cout << "FORMAT 1" << endl;
  for (unsigned i = 0; i < archives.size(); i++) {
    
    try {
      
      if (verbose)
	cerr << "Loading " << archives[i] << endl;
      
      arch = Pulsar::Archive::load(archives[i]);
      if (i==0 && gaussian)
	loadGaussian(gaussFile, stdarch, arch);
      if (fscrunch)
	arch->fscrunch();
      if (tscrunch)
	arch->tscrunch();
      if (full_poln) try {

	Pulsar::Integration* integration = arch->get_Integration(0);
	poln_profile = integration->new_PolnProfile(0);

	Tempo::toa toa = fit.get_toa (poln_profile,
				      integration->get_epoch(),
				      integration->get_folding_period(),
				      arch->get_telescope_code());

        string aux = basename( arch->get_filename() );
        toa.set_auxilliary_text (aux);

	toa.unload(stdout);

      }
      catch (Error& error) {
	cerr << "pat: Error while fitting " << error << endl;
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
	      stdarch = Pulsar::Archive::load(stdprofiles[j]);	      
	      if (j==0 || fabs(stdarch->get_centre_frequency() - freq)<minDiff)
		{
		  minDiff = fabs(stdarch->get_centre_frequency()-freq);
		  jDiff   = j;
		}
	    }
	  stdarch = Pulsar::Archive::load(stdprofiles[jDiff]);
	  stdarch->fscrunch();
	  stdarch->tscrunch();
	
          if (denoise)
            stdarch->denoise();
	
  
	  if (full_poln) {
	    
	    cerr << "pat: full polarization fitting with " << std << endl;
	    fit.set_standard( stdarch->get_Integration(0)->new_PolnProfile(0) );
	    fit.set_transformation( new Calibration::Polar );
	    
	  }
	  else
	    stdarch->convert_state(Signal::Intensity);
	}
	if (strcasecmp(outFormat.c_str(),"parkes")==0)
	  {
	    arch->toas(toas, stdarch, "", time_domain, Tempo::toa::Parkes); 
	  }
	else if (strcasecmp(outFormat.c_str(),"princeton")==0)
	  arch->toas(toas, stdarch, "", time_domain, Tempo::toa::Princeton); 
	else if (strcasecmp(outFormat.c_str(),"itoa")==0)
	  arch->toas(toas, stdarch, "", time_domain, Tempo::toa::ITOA); 
	else if (strcasecmp(outFormat.c_str(),"psrclock")==0)
	  arch->toas(toas, stdarch, "", time_domain, Tempo::toa::Psrclock); 
	else if (strcasecmp(outFormat.c_str(),"tempo2")==0)
	  {
	    string args;
	    args = archives[i];
	    if (outFormatFlags.find("i")!=string::npos) args += string(" -i ") + arch->get_backend();
	    if (outFormatFlags.find("r")!=string::npos) args += string(" -r ") + arch->get_receiver();
	    if (outFormatFlags.find("o")!=string::npos) /* Include observer info. */
	      {
		const Pulsar::ObsExtension* ext = 0;
		ext = arch->get<Pulsar::ObsExtension>();
		if (!ext) {
		  args += " -o N/A";
		}
		else {
		  args += " -o " + ext->observer;
		}
	      }
	    arch->toas(toas, stdarch, args, time_domain, Tempo::toa::Tempo2); 
	  }
      }
      for (unsigned i = 0; i < toas.size(); i++)
	toas[i].unload(stdout);
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


void loadGaussian(string file,  Reference::To<Pulsar::Archive> &stdarch,  Reference::To<Pulsar::Archive> arch)
{
  ifstream fin(file.c_str());
  string psr,str;
  char   buf[1000];
  float  tau,freq,spos,swidth,samp;
  static vector<float> pos,width,amp;
  static unsigned int    ncomp;
  float version;
  int    n=0;
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
	  cout << "Warning: number of Gaussian components " << 
	    ncomp << " does not equal components provided " << n << endl;
	  ncomp = n;
	}
    }
 
  stdarch = arch->clone();
  stdarch->fscrunch();
  stdarch->tscrunch();
  stdarch->pscrunch();
  
  // Now replace profile with Gaussian components
  Reference::To<Pulsar::Profile> prof;
  float *amps;
  float x,y;
  unsigned int i,j;

  prof = stdarch->get_Profile(0,0,0);
  amps = prof->get_amps();
  for (i=0;i<stdarch->get_nbin();i++)
    {
      x = (float)i*360.0/(float)stdarch->get_nbin();
      y = 0.0;
      for (j=0;j<ncomp;j++)
	y+=amp[j]*exp(-pow(x-pos[j],2)/pow(width[j],2));
      amps[i]=y;
      if (firstTime) cout << i << " " << y << " PROFILE " << endl;
    }
  prof->set_amps(amps);
  firstTime=false;
}
