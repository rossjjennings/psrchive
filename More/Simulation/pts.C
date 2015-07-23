/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include<config.h>
#endif

#include "Pulsar/SimPolnTiming.h"
#include "Pulsar/Archive.h"
#include "MEAL/ScalarParameter.h"
#ifdef HAVE_PGPLOT
#include <cpgplot.h>
#endif

#include <fstream>
#include <unistd.h>

using namespace Pulsar;
using namespace MEAL;
using namespace std;

void usage ()
{
  cout <<
    "pts: polarimetric timing simulator\n"
    "USAGE: pts [options]\n"
    "\n"
    "Total intensity gaussian options:\n"
    "  -C centre              specified in turns\n"
    "  -W width               specified in turns\n"
    "  -H height\n"
    "\n"
    "Linear Polarization (RVM) options:\n"
    "  -S line_of_sight       colatitude in degrees\n"
    "  -M magnetic_axis       colatitude in degrees\n"
    "  -B line_of_sight       to magnetic axis in degrees\n"
    "  -D fraction            degree of polarization (0 to 1)\n"
    "\n"
    "Parameter space options:\n"
    "  -a steps               extrapolate benefit to zero noise\n"
    "  -s steps[:range:index] vary the line of sight slope\n"
    "  -w steps[:range:index] vary the width of the gaussian\n"
    "  -g steps[:range:index] vary the differential gain\n"
    "  -p steps[:range:index] vary the parallactic angle\n"
    "\n"
    "Other options:\n"
    "  -A filename            standard archive filename\n"
    "  -c                     choose maximum harmonic\n"
    "  -F max_harmonic        maximum number of harmonics to use\n"
    "  -G diff_gain           differential gain of receiver\n"
    "  -N noise               r.m.s. of measurement noise\n"
    "  -n noise               r.m.s. of noise in standard\n"
    "\n";
#if HAVE_PGPLOT
    cout <<
    "Plot options:\n"
    "  -t                     no title along top\n"
    "  -d device              pgplot device\n"
       << endl;
#endif
}



int main (int argc, char** argv) try {

  const float twopi = 2.0 * M_PI;

  // the synthetic profile generator
  SimPolnTiming simulate;

  // extrapolate the benefit to account for noise in the standard
  unsigned stdrms_steps = 0;

  simulate.title = "Simulated Profile";
  string device = "?";
  bool verbose = false;

  const char* args = "A:a:B:b:C:cD:d:eF:f:G:g:H:hi:k:K:lM:N:n:O:p:S:s:tVvW:w:";
  char c;

  ofstream output;

  while ((c = getopt(argc, argv, args)) != -1) {

    switch (c)  {

    case 'A':
      cerr << "pts: Loading " << optarg << endl;
      simulate.set_standard (Pulsar::Archive::load( optarg ));
      break;

    case 'a':
      stdrms_steps = atoi(optarg);
      break;

    case 'B':
      simulate.rvm_beta = atof(optarg)*M_PI/180;
      break;

    case 'b':
      simulate.nbin = atoi (optarg);
      break;

    case 'C':
      simulate.set_centre (atof(optarg)*twopi);
      break;

    case 'c':
      simulate.fit.choose_maximum_harmonic = true;
      break;

    case 'd':
      device = optarg;
      break;

    case 'D':
      simulate.generate.set_degree (atof(optarg));
      break;

    case 'E':
      simulate.as_Estimate = false;
      break;

    case 'F':
      simulate.fit.set_maximum_harmonic( atoi(optarg) );
      break;

    case 'f':
      output.open (optarg, ios::out);
      if (!output)
	cerr << "Could not open " << optarg << endl;
      simulate.output = &output;
      break;

    case 'G':
      simulate.diff_gain = atof(optarg);
      break;

    case 'g':
      simulate.dgain.parse (optarg);
      break;

    case 'h':
      usage();
      return 0;

    case 'i':
      cerr << "Setting iterations to " << optarg << endl;
      simulate.iterations = atoi(optarg);
      break;

    case 'H':
      simulate.gaussian.set_height (atof(optarg));
      break;

    case 'K':
      simulate.boost_vector.parse (optarg);
      cerr << "will vary boost orientation " << simulate.boost_vector << endl;
      break;

    case 'k':
      simulate.beta = atof(optarg);
      cerr << "boost beta=" << simulate.beta << endl;
      break;

    case 'l':
      simulate.logarithmic = true;
      break;

    case 'm':
      simulate.slope.parse (optarg);
      break;

    case 'M':
      simulate.rvm.magnetic_axis->set_value (atof(optarg)*M_PI/180);
      break;

    case 'N':
      simulate.measurement_noise = atof(optarg);
      cerr << "measurement noise rms set to "
	   << simulate.measurement_noise << endl;
      break;

    case 'n':
      simulate.standard_noise = atof(optarg);
      cerr << "standard noise rms set to " 
	   << simulate.standard_noise << endl;
      break;

    case 'O':
      simulate.offset = atof(optarg);
      cerr << "pulse phase offset set to " 
	   << simulate.offset << " turns" << endl;
      simulate.offset *= twopi;
      break;

    case 'p': {
      simulate.parallactic.parse (optarg);
      break;
    }

    case 'S':
      simulate.rvm.line_of_sight->set_value (atof(optarg)*M_PI/180);
      break;

    case 's':
      simulate.vary_line_of_sight = true;
      simulate.slope.parse (optarg);
      cerr << "will vary tangent slope " << simulate.slope << endl;
      break;

    case 't':
      simulate.title = "";
      break;

    case 'V':
      Pulsar::Archive::set_verbosity (3);
    case 'v':
      verbose = true;
      break;

    case 'W':      
      simulate.gaussian.set_width  (atof(optarg)*twopi);
      break;

    case 'w':
      simulate.width.parse (optarg);
      cerr << "will vary width " << simulate.width << endl;
      break;

    } 
  }

  cerr << "iterations = " << simulate.iterations << endl;

#ifdef HAVE_PGPLOT
  cpgopen (device.c_str());
  cpgsvp (.1,.9, .1,.9);
  // separate the PGPLOT device prompt from the data
  if (device == "?")
    cout << endl;
#endif

  if (stdrms_steps) {
    cerr << "Extrapolate benefit to zero noise" << endl;
    simulate.extrapolate_benefit (stdrms_steps);
  }
  else if (simulate.parallactic.steps > 1)
    simulate.susceptibility ();
  else if (simulate.boost_vector.steps > 1)
    simulate.boost_around();
  else
    simulate.run_simulation ();
    
#ifdef HAVE_PGPLOT
  cpgend();
#endif

  return 0;

}
catch (Error& error) {
  cerr << "pts: " << error << endl;
  return -1;
}
