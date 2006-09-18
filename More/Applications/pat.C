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

#include "Pulsar/PolnProfile.h"
#include "Pulsar/PolnProfileFit.h"
#include "Pulsar/PolnProfileFitAnalysis.h"

#include "MEAL/Polar.h"

#include "Pulsar/ObsExtension.h"
#include "Pulsar/Backend.h"
#include "Pulsar/Receiver.h"

#include "Phase.h"
#include "toa.h"
#include "Error.h"
#include "dirutil.h"
#include "genutil.h"

#include <fstream>
#include <iostream>

#include <math.h>
#include <string.h>
#include <unistd.h>

void loadGaussian(string file,  
		  Reference::To<Pulsar::Archive> &stdarch,  
		  Reference::To<Pulsar::Archive> arch);

void usage ()
{
  cout << "pat - Pulsar::Archive timing \n"
    "Usage: pat [options] filenames \n"
    "  -q               Quiet mode \n"
    "  -v               Verbose mode \n"
    "  -V               Very verbose mode \n"
    "  -i               Show revision information \n"
    "\n"
    "Preprocessing options:\n"
    "  -F               Frequency scrunch before fitting \n"
    "  -T               Time scrunch before fitting \n"
    "\n"
    "Fitting options:\n"
    "  -a stdfiles      Automatically select standard from specified group\n"
    "  -c               Choose the maximum harmonic \n"
    "  -D               Denoise standard \n"
    "  -g datafile      Gaussian model fitting \n"
    "  -n harmonics     Use up to the specified number of harmonics\n"
    "  -p               Perform full polarimetric fit in Fourier domain \n"
    "  -P               Print a report on the polarimetric standard \n"
    "  -s stdfile       Location of standard profile \n"
    "  -S period        Zap harmonics due to periodic spikes in profile \n"
    "                   (use of this option implies SIS) \n"
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
    "  -f \"fmt <flags>\"  Select output format [default: parkes]\n"
    "                   Available formats: parkes tempo2, itoa, princeton \n"
    "                   For tempo2, <flags> include i = display instrument \n"
    "                                               r = display receiver   \n"
    "\n"
    "See "PSRCHIVE_HTTP"/manuals/pat for more details\n"
       << endl;
}

using namespace Pulsar;

// defined in mtm_analysis.C
void mtm_analysis (PolnProfileFitAnalysis&, PolnProfileFit&,
		   const std::string& name, bool optimal);

int main (int argc, char *argv[]) try {
  
  bool verbose = false;
  bool std_given = false;
  bool std_multiple = false;
  bool gaussian = false;
  bool full_poln = false;
  bool full_poln_analysis = false;

  bool fscrunch = false;
  bool tscrunch = false;

  bool denoise = false;
  string std,gaussFile;
  string outFormat("parkes"),outFormatFlags;

  vector<string> archives;
  vector<string> stdprofiles;
  vector<Tempo::toa> toas;

  Reference::To<Archive> arch;
  Reference::To<Archive> stdarch;
  Reference::To<Profile> prof;

  int gotc = 0;

  PolnProfileFitAnalysis analysis;
  PolnProfileFit fit;

  bool optimize_mtm = false;
  float chisq_max = 2.0;

  while ((gotc = getopt(argc, argv, "a:A:b:cDf:Fg:hin:oO:pPqS:s:tTvVx:")) != -1) {
    switch (gotc) {
    case 'h':
      usage ();
      return 0;

    case 'q':
      Archive::set_verbosity(0);
      break;

    case 'v':
      Archive::set_verbosity(2);
      verbose = true;
      break;

    case 'V':
      verbose = true;
      Archive::set_verbosity(3);
      MEAL::Function::verbose = true;
      Pulsar::PolnProfileFit::verbose = true;
      break;

    case 'b':
      analysis.set_optimal_boost_max (atof(optarg));
      break;

    case 'c':
      fit.choose_maximum_harmonic = true;
      break;

    case 'D':
      denoise = true;
      break;

    case 'i':
      cout << "$Id: pat.C,v 1.65 2006/09/18 13:25:01 straten Exp $" << endl;
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

    case 'o':
      optimize_mtm = true;
      break;

    case 'O':
      analysis.set_optimal_harmonic_max( atoi(optarg) );
      break;

    case 'P':
      full_poln_analysis = true;
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

    case 'S':
      Profile::shift_strategy.set(&SincInterpShift);      
      Profile::SIS_zap_period = atoi(optarg);
      break;

    case 's':
      std_given = true;
      std = optarg;
      break;

    case 'f':
      {
	outFormat = string(optarg).substr(0,string(optarg).find(" "));  /* Set the output format */
	if (string(optarg).find(" ")!=string::npos) outFormatFlags = string(optarg).substr(string(optarg).find(" "));
	break;
      }

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

  for (int ai=optind; ai<argc; ai++)
    dirglob (&archives, argv[ai]);
  
  if (!full_poln_analysis && archives.empty()) {
    cerr << "No archives were specified" << endl;
    return -1;
  } 
  
  if (!std_given && !gaussian) {
    cerr << "You must specify the standard profile to use!" << endl;
    return -1;
  }

  Reference::To<PolnProfile> poln_profile;

  if (!std_multiple && !gaussian) try {

    // If only using one standard profile ...

    stdarch = Archive::load(std);
    stdarch->fscrunch();
    stdarch->tscrunch();
    
    if (denoise)
      stdarch->denoise();
    
    if (full_poln) {
      
      cerr << "pat: using full polarization" << endl;
      fit.set_standard( stdarch->get_Integration(0)->new_PolnProfile(0) );
      fit.set_transformation( new MEAL::Polar );
      cerr << "pat: last harmonic = " << fit.get_nharmonic() << endl;

    }
    else
      stdarch->convert_state(Signal::Intensity);

    if (full_poln_analysis || optimize_mtm) {

      // for the invariant interval analysis
      stdarch->remove_baseline();

      cerr << "pat: performing full polarization analysis" << endl;
      mtm_analysis (analysis, fit, stdarch->get_source(), optimize_mtm);

    }

  }
  catch (Error& error) {
    cerr << "Error processing standard profile:" << endl;
    cerr << error << endl;
    return -1;
  }

  // Give format information for Tempo2 output 
  if (strcasecmp(outFormat.c_str(),"tempo2")==0)
    cout << "FORMAT 1" << endl;

  // optimization: remember the last successful MTM fit
  MEAL::Polar backup;

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

      if (full_poln) try {

	Integration* integration = arch->get_Integration(0);
	poln_profile = integration->new_PolnProfile(0);

        fit.get_transformation()->copy( &backup );

        if (optimize_mtm)
          analysis.use_basis (false);

	Tempo::toa toa = fit.get_toa (poln_profile,
				      integration->get_epoch(),
				      integration->get_folding_period(),
				      arch->get_telescope_code());

        backup.copy( fit.get_transformation() );

	if (optimize_mtm) {

	  Jones<double> xform = fit.get_transformation()->evaluate();

	  analysis.use_basis (true);

	  if (analysis.has_Jbasis()) {
	    Jones<double> basis = analysis.get_Jbasis()->evaluate();
	    poln_profile->transform( basis * inv(xform) );
	  }
	  else {
	    Matrix<4,4,double> basis = analysis.get_Mbasis()->evaluate();
	    Matrix<4,4,double> best = Mueller( inv(xform) );
	    poln_profile->transform( basis * best );
	  }

          MEAL::Polar identity;
          fit.get_transformation()->copy( &identity );

	  toa = fit.get_toa (poln_profile,
			     integration->get_epoch(),
			     integration->get_folding_period(),
			     arch->get_telescope_code());

	}

        string aux = basename( arch->get_filename() );
        float chisq = fit.get_fit_chisq() / fit.get_fit_nfree();

        if (verbose)
          cerr << "pat: " << aux << " chisq=" << chisq << endl;

	if (chisq_max == 0.0 || chisq < chisq_max) {

	  if (arch->get_dedispersed())
	    toa.set_frequency (arch->get_centre_frequency());
	  
	  toa.set_auxilliary_text (aux);
	  
	  toa.unload(stdout);

	}

      }
      catch (Error& error) {
	cerr << "pat: Error MTM " << arch->get_filename() << endl;
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
	
          if (denoise)
            stdarch->denoise();
	
  
	  if (full_poln) {
	    
	    cerr << "pat: full polarization fitting with " << std << endl;
	    fit.set_standard( stdarch->get_Integration(0)->new_PolnProfile(0) );
	    fit.set_transformation( new MEAL::Polar );
	    
	  }
	  else
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
		args += string(" -i ") + receiver->get_name();
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


