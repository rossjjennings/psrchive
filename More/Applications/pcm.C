//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Applications/pcm.C,v $
   $Revision: 1.4 $
   $Date: 2003/10/13 13:57:53 $
   $Author: straten $ */

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
    "Usage: pcm [options] filenames\n"
    "  -a archive set the output archive class name\n"
    "  -b bin     add phase bin to constraints\n"
    "  -C meta    filename with list of calibrator files\n"
    "  -f chan    solve for only the specified channel\n"
    "  -m model   model: Britton [default] or Hamaker\n"
    "  -M meta    filename with list of pulsar files\n"
    "  -n nbin    set the number of phase bins to use as input states\n"
    "  -p pA,pB   set the phase window from which to take input states\n"
    "  -u         assume that CAL Stokes V = 0\n"
    "  -v         verbose mode\n"
    "  -V         very verbose mode\n"
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
  
  cerr << "pcm: selecting " << total_bins 
       << " phase bin constraints from " << rise << " to " << fall 
       << " (nbin=" << nbin << ")" << endl;
  
  unsigned last_bin = fall;
  
  for (float bin = rise; bin<=fall; bin += increment) {
    unsigned ibin = unsigned(bin) % nbin;
    if (ibin != last_bin)  {
      cerr << "pcm: adding phase bin " << ibin << endl;
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
      cerr << "pcm: adding phase bin " << ibin << endl;
      model.add_state (ibin%nbin);
      last_bin = ibin;
    }
  }
}


void plot_constraints (Pulsar::ReceptionCalibratorPlotter& plotter,
		       unsigned nchan, unsigned nstate, unsigned nplot,
		       unsigned start_state, unsigned only_chan=0)
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
    
    cerr << "pcm: nstate=" << nstate << endl;
    for (unsigned istate=0; istate<nstate; istate++) {

      unsigned plot_state = istate+start_state;

      cerr << "ichan=" << ichan << " istate=" << plot_state << endl;
      plotter.plot_constraints (ichan, plot_state);

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
try {
  Error::verbose = false;

  // name of file containing list of Archive filenames
  char* metafile = NULL;

  // name of file containing list of Archive filenames
  char* calfile = NULL;

  // name of the default parameterization
  Pulsar::Calibrator::Type model_name = Pulsar::Calibrator::Britton;

  // class name of the calibrator solution archives to be produced
  string archive_class = "FITSArchive";

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

  bool measure_cal_V = true;

  int gotc = 0;
  while ((gotc = getopt(argc, argv, "a:b:C:Df:M:m:n:p:huvV")) != -1) {
    switch (gotc) {

    case 'a':
      archive_class = optarg;
      break;

    case 'b': {
      unsigned bin = atoi (optarg);
      cerr << "pcm: adding phase bin " << bin << endl;
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
      cerr << "pcm: solving only channel " << only_ichan << endl;
      break;

    case 'm':
      if (optarg == Britton)
	model_name = Pulsar::Calibrator::Britton;
      else if (optarg == Hamaker)
	model_name = Pulsar::Calibrator::Hamaker;
      else {
	cerr << "pcm: unrecognized model name '" << optarg << "'" << endl;
	return -1;
      }
      break;
      
    case 'M':
      metafile = optarg;
      break;

    case 'n':
      maxbins = atoi (optarg);
      cerr << "pcm: selecting a maximum of " << maxbins << " bins" << endl;
      break;

    case 'p':
      if (sscanf (optarg, "%f,%f", &phmin, &phmax) != 2) {
	cerr << "pcm: error parsing " << optarg << " as phase window" << endl;
	return -1;
      }
      cerr << "pcm: selecting input states from " << phmin << " to " << phmax
	   << endl;
      break;

    case 'h':
      usage ();
      return 0;

    case 'u':
      measure_cal_V = false;
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
    cerr << "pcm: no archives were specified" << endl;
    return -1;
  } 

  // the reception calibration class
  Pulsar::ReceptionCalibrator model (model_name);

  if (measure_cal_V)
    cerr << "pcm: assuming that System + Hydra A Stokes V = 0" << endl;

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
  
  cerr << "pcm: loading archives" << endl;
  
  for (unsigned i = 0; i < filenames.size(); i++) {
    
    try {
      
      if (verbose)
	cerr << "pcm: loading " << filenames[i] << endl;
      
      archive = Pulsar::Archive::load(filenames[i]);
      
      cout << "pcm: loaded archive: " << filenames[i] << endl;
      
      if (archive->get_type() == Signal::Pulsar)  {
	
	if (verbose)
	  cerr << "pcm: dedispersing and removing baseline from pulsar data"
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

        cerr << "pcm: " << model.get_nstate_pulsar() << " states" << endl;
      }

      model.add_observation( archive );


      if (archive->get_type() == Signal::Pulsar && only_ichan < 0)  {

	if (verbose)
	  cerr << "pcm: calibrate with current best guess" << endl;

	model.precalibrate (archive);

	if (verbose)
	  cerr << "pcm: fscrunch, deparallactify, and add to total" << endl;

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

    cerr << "pcm: plotting initial guess of receiver" << endl;
    plotter.plot (&model);

    cpgpage();

    cerr << "pcm: plotting uncalibrated pulsar total stokes" << endl;
    Pulsar::Plotter profile;
    profile.Manchester (total);

    cpgpage();

    cerr << "pcm: plotting uncalibrated CAL" << endl;
    plotter.plotcal();

    cpgend();

    cerr << "pcm: plotting pulsar constraints" << endl;
    plot_constraints (plotter, model.get_nchan(),
		      model.get_nstate_pulsar(), 12, 
		      model.get_nstate()-model.get_nstate_pulsar());


  }

  total = 0;

  cerr << "pcm: solving model" << endl;

  try {
    model.solve (only_ichan);
  }
  catch (Error& error) {
    cerr << error << endl;
    return -1;
  }

  Reference::To<Pulsar::Archive> solution = model.get_solution (archive_class);

  cerr << "psc: unloading solution to " << solution->get_filename() << endl;
  solution->unload( "pcm.fits" );

  if (display) {

    cpgbeg (0, "result.ps/CPS", 0, 0);
    cpgask(1);
    cpgsvp (0.1,.9, 0.1,.9);

    cerr << "pcm: plotting model receiver" << endl;
    plotter.plot (&model);

    cpgpage();

    cerr << "pcm: plotting calibrated CAL" << endl;
    plotter.plotcal();

    cpgend ();

    cerr << "pcm: plotting pulsar constraints with model" << endl;
    plot_constraints (plotter, model.get_nchan(),
		      model.get_nstate_pulsar(), 12,
		      model.get_nstate()-model.get_nstate_pulsar());

  }

  for (unsigned ical=0; ical < cal_filenames.size(); ical++)
    dirglob (&filenames, cal_filenames[ical]);
  
  cerr << "pcm: calibrating archives (PSR and CAL)" << endl;

  for (unsigned i = 0; i < filenames.size(); i++) {
    
    try {

      if (verbose)
	cerr << "pcm: loading " << filenames[i] << endl;

      archive = Pulsar::Archive::load(filenames[i]);

      cout << "pcm: loaded archive: " << filenames[i] << endl;
      
      model.precalibrate( archive );

      if (archive->get_type() == Signal::Pulsar)
	archive->deparallactify ();

      int index = filenames[i].find_first_of(".", 0);
      string newname = filenames[i].substr(0, index);
      newname += ".calib";

      if (verbose)
        cerr << "pcm: calibrated Archive name '" << newname << "'" << endl;

      archive->unload (newname);
      
      cout << "New file " << newname << " unloaded" << endl;

      if (archive->get_type() == Signal::Pulsar)  {

        if (verbose)
          cerr << "pcm: fscrunch and add to calibrated total" << endl;

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
    cerr << "pcm: writing total integrated pulsar archive" << endl;
    total->unload ("total.ar");
  }

  if (display) {

    cpgbeg (0, "calibrated.ps/CPS", 0, 0);
    cpgask(1);
    cpgsvp (0.1,.9, 0.1,.9);

    cerr << "pcm: plotting calibrated pulsar total stokes" << endl;
    Pulsar::Plotter profile;
    profile.Manchester (total);

    cpgend ();
  }

  return 0;
}
catch (Error& error) {
  cerr << "pcm: error" << error << endl;
  return -1;
}

