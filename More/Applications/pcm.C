
/*! \file pcm.C 
    \brief (Pulsar) Polarimetric Calibration Modelling (Package)

    This program may be used to model the polarimetric response of observatory
    instrumentation using observations of:

    <UL>
    <LI> a single pulsar at multiple parallactic angles
    <LI> the amplitude modulated linear noise diode
    <LI> an unpolarized flux calibrator, such as Hydra A
    </UL>

*/

#include "Pulsar/ReceptionCalibratorPlotter.h"
#include "Pulsar/ReceptionCalibrator.h"
#include "Pulsar/Plotter.h"
#include "Pulsar/Archive.h"

#include "Error.h"
#include "dirutil.h"
#include "string_utils.h"

#include <cpgplot.h>

#include <iostream>
#include <unistd.h>

static string Britton = "Britton";
static string Hamaker = "Hamaker";

void usage ()
{
  cout << "A program for performing self-calibration\n"
    "Usage: psc [options] filenames\n"
    "  -a bin    Add phase bin to constraints\n"
    "  -C meta   filename with list of calibrator files\n"
    "  -f chan   solve for only the specified channel\n"
    "  -m model  Britton [default] or Hamaker\n"
    "  -M meta   filename with list of pulsar files\n"
    "  -n nbin   set the number of phase bins to use as input states\n"
    "  -p pA,pB  Set the phase window from which to take input states\n"
    "  -u        Assume that Hydra is unpolarized\n"
    "  -v        Verbose mode\n"
    "  -V        Very verbose mode\n"
       << endl;
}

void auto_select (Pulsar::ReceptionCalibrator& model, Pulsar::Archive* archive,
		  unsigned maxbins)
{
  int rise = 0, fall = 0;
  archive->find_peak_edges (rise, fall);
  
  float increment = 1.0;
  
  unsigned total_bins = fall - rise;
  
  if (total_bins > maxbins)  {
    increment = float(total_bins+1) / float (maxbins);
    total_bins = maxbins;
  }
  
  unsigned nbin = archive->get_nbin();
  
  cerr << "psc: selecting " << total_bins 
       << " phase bin constraints from " << rise << " to " << fall 
       << " (nbin=" << nbin << ")" << endl;
  
  unsigned last_bin = fall;
  
  for (float bin = rise; bin<=fall; bin += increment) {
    unsigned ibin = unsigned(bin) % nbin;
    if (ibin != last_bin)  {
      cerr << "psc: adding phase bin " << ibin << endl;
      model.add_state (ibin%nbin);
      last_bin = ibin;
    }
  }
}
void range_select (Pulsar::ReceptionCalibrator& model,
		   Pulsar::Archive* archive,
		   float phmin, float phmax, unsigned maxbins)
{
  unsigned nbin = archive->get_nbin();

  float increment = (phmax - phmin)/maxbins;

  unsigned last_bin = unsigned (phmax * nbin);

  for (float bin = phmin; bin<=phmax; bin += increment) {

    unsigned ibin = unsigned (bin * nbin);

    if (ibin != last_bin)  {
      cerr << "psc: adding phase bin " << ibin << endl;
      model.add_state (ibin%nbin);
      last_bin = ibin;
    }
  }
}


void plot_constraints (Pulsar::ReceptionCalibratorPlotter& plotter,
		       unsigned nchan, unsigned nstate, unsigned nplot,
		       unsigned only_chan = 0)
{
  unsigned incr = nchan/nplot;
  if (!incr)
    incr = 1;

  unsigned ichan = 1;

  if (nplot == 1)
    ichan = only_chan;

  if (nstate == 1)
    cpgbeg (0, "states.ps/CPS", 0, 0);

  for (; ichan < nchan; ichan+=incr) {

    if (nstate > 1) {
      char filename [256];
      sprintf (filename, "channel_%d.ps/CPS", ichan);
      cpgbeg (0, filename, 0, 0);
    }

    cpgsvp (.1,.9, .1,.9);
    
    cerr << "psc: nstate=" << nstate << endl;
    for (unsigned istate=0; istate<nstate; istate++) {
      cerr << "ichan=" << ichan << " istate=" << istate+1 << endl;
      plotter.plot_constraints (ichan, istate+1);

      if (istate+1 < nstate)
	cpgpage();
    }

    if (nstate > 1)
      cpgend ();
    else if (ichan+incr < nchan)
      cpgpage ();

  }

  if (nstate == 1)
    cpgend ();

}



int main (int argc, char *argv[]) 
{
  Error::verbose = false;

  // name of file containing list of Archive filenames
  char* metafile = NULL;

  // name of file containing list of Archive filenames
  char* calfile = NULL;

  // name of the default parameterization
  Pulsar::Calibrator::Type model_name = Pulsar::Calibrator::Britton;

  // plot the solution before calibrating with it
  bool display = true;

  // verbosity flags
  bool verbose = false;

  //! The maximum number of bins to use
  unsigned maxbins = 16;

  //! The pulse phase window to use
  float phmin, phmax;

  phmin = phmax = 0.0;

  //! The channel to solve (-f specified)
  int only_ichan = -1;

  //! The phase bins to add to the model
  vector<unsigned> phase_bins;

  bool measure_cal_V = false;

  int gotc = 0;
  while ((gotc = getopt(argc, argv, "a:C:Df:M:m:n:p:huvV")) != -1) {
    switch (gotc) {

    case 'a': {
      unsigned bin = atoi (optarg);
      cerr << "psc: adding phase bin " << bin << endl;
      phase_bins.push_back (bin);
      break;
    }

    case 'C':
      calfile = optarg;
      break;

    case 'D':
      display = false;
      break;

    case 'f':
      only_ichan = atoi (optarg);
      cerr << "psc: solving only channel " << only_ichan << endl;
      break;

    case 'm':
      if (optarg == Britton)
	model_name = Pulsar::Calibrator::Britton;
      else if (optarg == Hamaker)
	model_name = Pulsar::Calibrator::Hamaker;
      else {
	cerr << "psc: unrecognized model name '" << optarg << "'" << endl;
	return -1;
      }
      break;
      
    case 'M':
      metafile = optarg;
      break;

    case 'n':
      maxbins = atoi (optarg);
      cerr << "psc: selecting a maximum of " << maxbins << " bins" << endl;
      break;

    case 'p':
      if (sscanf (optarg, "%f,%f", &phmin, &phmax) != 2) {
	cerr << "psc: error parsing " << optarg << " as phase window" << endl;
	return -1;
      }
      cerr << "psc: selecting input states from " << phmin << " to " << phmax
	   << endl;
      break;

    case 'h':
      usage ();
      return 0;

    case 'u':
      measure_cal_V = true;
      break;

    case 'v':
      verbose = true;
      break;

    case 'V':
      verbose = true;
      Calibration::ReceptionModel::verbose = true;
      Pulsar::ReceptionCalibrator::verbose = true;
      Pulsar::Archive::verbose = true;
      break;


    default:
      cout << "Unrecognised option" << endl;
    }
  }
  
  vector <string> filenames;

  if (metafile)
    stringfload (&filenames, metafile);
  for (int ai=optind; ai<argc; ai++)
    dirglob (&filenames, argv[ai]);
  
  if (filenames.empty()) {
    cerr << "psc: no archives were specified" << endl;
    return -1;
  } 

  // the reception calibration class
  Pulsar::ReceptionCalibrator model (model_name);

  if (measure_cal_V)
    cerr << "psc: Assuming that Hydra is unpolarized" << endl;

  model.measure_cal_V = measure_cal_V;

  // add the specified phase bins
  for (unsigned ibin=0; ibin<phase_bins.size(); ibin++)
    model.add_state (phase_bins[ibin]);


  // add the calibrators (to be loaded on first call to add_observation
  vector<string> cal_filenames;
  if (calfile) {
    stringfload (&cal_filenames, calfile);
    model.set_calibrators (cal_filenames);
  }
  
  
  Reference::To<Pulsar::Archive> total;
  Reference::To<Pulsar::Archive> archive;
  
  cerr << "psc: loading archives" << endl;
  
  for (unsigned i = 0; i < filenames.size(); i++) {
    
    try {
      
      if (verbose)
	cerr << "psc: loading " << filenames[i] << endl;
      
      archive = Pulsar::Archive::load(filenames[i]);
      
      cout << "psc: loaded archive: " << filenames[i] << endl;
      
      if (archive->get_type() == Signal::Pulsar)  {
	
	if (verbose)
	  cerr << "psc: dedispersing and removing baseline from pulsar data"
               << endl;
	
        archive->dedisperse ();
	archive->convert_state (Signal::Stokes);
        archive->remove_baseline ();
      }

      if (model.get_nstate_pulsar() == 0) {

	if (phmin == phmax)
	  auto_select (model, archive, maxbins);

	else
	  range_select (model, archive, phmin, phmax, maxbins);

        cerr << "psc: " << model.get_nstate_pulsar() << " states" << endl;
      }

      model.add_observation( archive );


      if (archive->get_type() == Signal::Pulsar && only_ichan < 0)  {

	if (verbose)
	  cerr << "psc: calibrate with current best guess" << endl;

	model.precalibrate (archive);

	if (verbose)
	  cerr << "psc: fscrunch, deparallactify, and add to total" << endl;

        archive->fscrunch ();
        archive->deparallactify ();

	if (!total)
	  total = archive;
	else {
	  total->append (archive);
	  total->tscrunch ();
	}
      }

    }
    catch (Error& error) {
      cerr << error << endl;
    }
  }

  Pulsar::ReceptionCalibratorPlotter plotter (&model);

  if (display && only_ichan < 0) {


    cpgbeg (0, "guess.ps/CPS", 0, 0);
    cpgask(1);
    cpgsvp (.1,.9, .1,.9);

    cerr << "psc: plotting initial guess of receiver" << endl;
    plotter.plot (&model);

    cpgpage();

    cerr << "psc: plotting uncalibrated pulsar total stokes" << endl;
    Pulsar::Plotter profile;
    profile.Manchester (total);

    cpgpage();

    cerr << "psc: plotting uncalibrated CAL" << endl;
    plotter.plotcal();

    cpgend();

    cerr << "psc: plotting pulsar constraints" << endl;
    plot_constraints (plotter, model.get_nchan(),
		      model.get_nstate_pulsar(), 12);


  }

  total = 0;

  cerr << "psc: solving model" << endl;

  try {
    model.solve (only_ichan);
  }
  catch (Error& error) {
    cerr << error << endl;
    return -1;
  }


  if (display) {

    cpgbeg (0, "result.ps/CPS", 0, 0);
    cpgask(1);
    cpgsvp (0.1,.9, 0.1,.9);

    cerr << "psc: plotting model receiver" << endl;
    plotter.plot (&model);

    cpgpage();

    cerr << "psc: plotting calibrated CAL" << endl;
    plotter.plotcal();

    cpgend ();

    cerr << "psc: plotting pulsar constraints with model" << endl;
    plot_constraints (plotter, model.get_nchan(),
		      model.get_nstate_pulsar(), 12);

  }

  for (unsigned ical=0; ical < cal_filenames.size(); ical++)
    dirglob (&filenames, cal_filenames[ical]);
  
  cerr << "psc: calibrating archives (PSR and CAL)" << endl;

  for (unsigned i = 0; i < filenames.size(); i++) {
    
    try {

      if (verbose)
	cerr << "psc: loading " << filenames[i] << endl;

      archive = Pulsar::Archive::load(filenames[i]);

      cout << "psc: loaded archive: " << filenames[i] << endl;
      
      model.precalibrate( archive );

      if (archive->get_type() == Signal::Pulsar)
	archive->deparallactify ();

      int index = filenames[i].find_first_of(".", 0);
      string newname = filenames[i].substr(0, index);
      newname += ".calib";

      if (verbose)
        cerr << "psc: Calibrated Archive name '" << newname << "'" << endl;

      archive->unload (newname);
      
      cout << "New file " << newname << " unloaded" << endl;

      if (archive->get_type() == Signal::Pulsar)  {

        if (verbose)
          cerr << "psc: Fscrunch and add to calibrated total" << endl;

        archive->fscrunch ();
        archive->deparallactify ();

        if (!total)
          total = archive;
        else {
          total->append (archive);
          total->tscrunch ();
        }
      }

    }
    catch (Error& error) {
      cerr << error << endl;
    }
  }

  if (total)  {
    cerr << "psc: writing total integrated pulsar archive" << endl;
    total->unload ("total.ar");
  }

  if (display) {

    cpgbeg (0, "calibrated.ps/CPS", 0, 0);
    cpgask(1);
    cpgsvp (0.1,.9, 0.1,.9);

    cerr << "psc: plotting calibrated pulsar total stokes" << endl;
    Pulsar::Plotter profile;
    profile.Manchester (total);

    cpgend ();
  }

}
