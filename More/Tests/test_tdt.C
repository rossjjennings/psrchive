#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include "dirutil.h"

#include <cmath>

#include <cpgplot.h>

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Plotter.h"
#include "toa.h"
#include "Error.h"
#include "minmax.h"

int main(int argc, char** argv) {
  
  bool display = false;
  bool verbose = false;

  bool stdflag = false;

  Reference::To<Pulsar::Archive> stdarch;
  Reference::To<Pulsar::Profile> stdprof;

  int gotc = 0;
  
  vector<string> archives;
  
  while ((gotc = getopt(argc, argv, "hvVDs:")) != -1) {
    switch (gotc) {
    case 'h':
      cout << "A program for testing the TimeDomain TOA algorithm" << endl;
      cout << "Usage: rmfit [options] filenames"                   << endl;
      cout << "  -h               This help page"                  << endl;
      cout << "  -v               Verbose mode"                    << endl;
      cout << "  -V               Very verbose mode"               << endl;
      cout << "  -D               Display results"                 << endl;
      cout << "  -s               Standard profile"                << endl;
      return (-1);
      break;
    case 'v':
      verbose = true;
      break;
    case 'V':
      verbose = true;
      Pulsar::Archive::set_verbosity(1);
      break;
    case 'D':
      display = true;
      break;
    case 's':
      try {
	stdarch = Pulsar::Archive::load(optarg);
	stdprof = new Pulsar::Profile(stdarch->total()->get_Profile(0,0,0));
	stdflag = true;
      }
      catch (Error& error) {
	cerr << error << endl;
	return (-1);
      }
      break;
      
    default:
      cout << "Unrecognised option" << endl;
    }
  }
  
  if (!stdflag) {
    cerr << "This program requires a standard profile!" << endl;
    return(-1);
  }

  // Parse the list of archives
  
  for (int ai=optind; ai<argc; ai++)
    dirglob (&archives, argv[ai]);
  
  if (archives.empty()) {
    cerr << "No archives were specified" << endl;
    exit(-1);
  }

  if (display)
    cpgopen("?");
  
  Reference::To<Pulsar::Archive> data;

  for (unsigned i = 0; i < archives.size(); i++) {
    
    // Load in a file

    try {
      
      data = Pulsar::Archive::load(archives[i]);

    }
    catch (Error& error) {
      cerr << error << endl;
      continue;
    }
    
    float* corr = new float[data->get_nbin()];
    float* bins = new float[data->get_nbin()];
    float* parb = new float[data->get_nbin()];
    float* fn   = new float[3];

    string args;

    Tempo::toa toa = data->total()->
      get_Profile(0,0,0)->tdt(*stdprof,
			      data->get_Integration(0)->get_epoch(),
			      data->get_Integration(0)->get_folding_period(),
			      data->get_telescope_code(), 
			      args, 
			      Tempo::toa::Parkes,
			      corr,
			      fn);
    toa.unload(stdout);

    for (unsigned i = 0; i < data->get_nbin(); i++) {
      bins[i] = float(i)/data->get_nbin();
      parb[i] = fn[0]*bins[i]*bins[i] + fn[1]*bins[i] + fn[2];
    }

    if (display) {

      cpgsch(1.5);

      cpgsubp(1,2);
      cpgpanl(1,1);

      cpgsvp(0.1,0.9,0.1,0.9);
      
      float ymin = 0.0;
      float ymax = 0.0;

      findminmax(corr, corr+data->get_nbin()-1, ymin, ymax);

      cpgswin(0.0,1.0,ymin,ymax);
      cpgbox ("BCNST", 0.0, 0, "BCNST", 0.0, 0);

      cpgline(data->get_nbin(), bins, corr);
      cpgsci(2);
      cpgline(data->get_nbin(), bins, parb);
      cpgsci(1);

      cpgpanl(1,2);
      cpgsvp(0.1,0.5,0.1,0.9);

      float maxphs = 0.0;
      int   maxbin = 0;
      ymax = 0.0;

      for (unsigned i = 0; i < data->get_nbin(); i++) {
	if (corr[i] > ymax) {
	  ymax = corr[i];
	  maxbin = i;
	  maxphs = float(i)/float(data->get_nbin());
	}
      }
      
      int binmin = maxbin - 3;
      int binmax = maxbin + 3;

      if (binmin < 0)
	binmin = 0;

      if (binmax > int(data->get_nbin())-1)
	binmax = data->get_nbin()-1;

      findminmax(&corr[binmin], &corr[binmax], ymin, ymax);

      float phsmin = bins[binmin];
      float phsmax = bins[binmax];

      cpgswin(phsmin, phsmax, ymin, ymax);
      cpgbox ("BCNST", 0.0, 0, "BCNST", 0.0, 0);
      
      for (int i = binmin; i < binmax; i++) {
	cpgpt1(bins[i], corr[i], 0);
      }

      float stepsize = (phsmax-phsmin) / float(data->get_nbin());
      
      for (unsigned i = 0; i < data->get_nbin(); i++) {
	bins[i] = phsmin + float(i)*stepsize;
	parb[i] = fn[0] - fn[1]*(bins[i]-fn[2])*(bins[i]-fn[2]);
	//parb[i] = fn[0]*bins[i]*bins[i] + fn[1]*bins[i] + fn[2];
      }

      cpgsci(2);
      cpgline(data->get_nbin(), bins, parb);
      cpgsci(3);

    }
  }
}



