/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/Archive.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/DeltaRM.h"
#include "Pulsar/PolnProfileStats.h"
#include "Pulsar/FaradayRotation.h"

#include "MEAL/LevenbergMarquardt.h"
#include "MEAL/Gaussian.h"
#include "MEAL/Polynomial.h"
#include "MEAL/Axis.h"

#include "dirutil.h"
#include "Error.h"
#include "strutil.h"
#include "templates.h"

#if HAVE_PGPLOT
#include "Pulsar/StokesCylindrical.h"
#include <cpgplot.h>
#endif

#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <string>
#include <cmath>

using namespace std;
using TextInterface::parse_indeces;

bool verbose = false;

void cpg_next ();

void do_refine(Reference::To<Pulsar::Archive> data,bool log_results);

Reference::To<Pulsar::Archive> get_data (string archive_filename);

double do_maxmthd(double minrm,double maxrm,unsigned rmsteps,
		  Reference::To<Pulsar::Archive> data);

// prints various numbers out to file
void
rmresult (Pulsar::Archive* archive, const Estimate<double>& rm, unsigned used);

static bool display = false;

void usage ()
{
  cout <<
    "A program for finding rotation measures\n"
    "Usage: rmfit [options] filenames\n"
    "\n"
    "Fourier method search options: \n"
    "  -m min,max,steps  Specify the bounds and number of steps \n"
    "  -t                Use the default bounds and resolution \n"
    "  -u max            Set the upper bound on the default maximum RM \n"
    "  -U rm/dm          Set upper bound on default maximum RM = DM * rm/dm \n"
    "\n"
    "Fit options: \n"
    "  -r                Refine the RM using two halves of band \n"
    "\n"
    "Preprocessing options: \n"
    "  -b [+/-][range]   Include/exclude phase bins \n"
    "  -B [factor]       Scrunch in phase bins \n"
    "  -F [factor]       Scrunch in frequency \n"
    "  -R RM             Set the input rotation measure \n"
    "  -z \"z1 z2 z3...\"  Zap these channels\n"
    "\n"
    "Plotting options: \n"
    "  -D                Display results \n"
    "  -K dev            Specify pgplot device\n"
    "\n"
    "Standard options:\n"
    "  -h                This help page \n"
    "  -v                Verbose mode \n"
    "  -V                Very verbose mode \n"
    "  -L                Log results in source.log \n"
       << endl;
}

static bool auto_maxmthd = false;
static float auto_maxrm_dm = 0.0;
static float auto_maxrm = 1500.0;
static float auto_minrm = 0.0;
static float auto_step_rad = 0.0;

static unsigned auto_minsteps = 10;
static float refine_threshold = 0;
static unsigned max_iterations = 10;

Estimate<double> best_search_rm;

static vector<unsigned> include_bins;
static vector<unsigned> exclude_bins;

int main (int argc, char** argv)
{
  bool refine  = false;
  bool maxmthd = false;

  float minrm = 0.0;
  float maxrm = 0.0;
  unsigned rmsteps = 50;

  bool fscrunchme = false;
  int fscr = 1;

  bool bscrunchme = false;
  int bscr = 1;

  float threshold = 0.01;

  bool log_results = false;

  bool rm_set = false;
  double rotation_measure = 0.0;

  string cpg_device = "?";
  float nsigma = 1.0;
  vector<unsigned> zap_chans;

  vector<string> archives;

  string exclude_range;
  string include_range;

  const char* args = "b:B:DeF:hJK:Lm:pPrR:S:T:tu:U:vVw:W:z:";
  int gotc = 0;

  while ((gotc = getopt(argc, argv, args)) != -1) {
    switch (gotc) {

    case 'b':
      switch (optarg[0])
	{
	case '-':
	  exclude_range = optarg + 1;
	  cerr << "rmfit: will exclude " << exclude_range << endl;
	  break;
	case '+':
	  include_range = optarg + 1;
	  cerr << "rmfit: will include " << include_range << endl;
	  break;
	default:
	  cerr << "rmfit: invalid bin inclusion/exclusion string "
	    "'" << optarg << "'" << endl;
	  return 0;
	}
      break;

    case 'B':
      bscrunchme = true;
      if (sscanf(optarg, "%d", &bscr) != 1) {
	cerr << "That is not a valid scrunch factor!" << endl;
	return -1;
      }
      break;

    case 'D':
      display = true;
      break;

    case 'F':
      fscrunchme = true;
      if (sscanf(optarg, "%d", &fscr) != 1) {
	cerr << "That is not a valid scrunch factor!" << endl;
	return -1;
      }
      break;

    case 'h':
      usage ();
      return -1;
      break;

    case 'K':
      cpg_device = optarg;
      break;

    case 'L':
      log_results = true;
      break;

    case 'm':
      maxmthd = true;
      if (sscanf(optarg, "%f,%f,%ud", &minrm, &maxrm, &rmsteps) != 3) {
	cerr << "Invalid paramaters!" << endl;
	return -1;
      }
      if ((maxrm < minrm) || (rmsteps < 2)) {
	cerr << "Invalid paramaters!" << endl;
	return -1;
      }
      break;

    case 'r':
      refine = true;
      break;

    case 'R':
      rotation_measure = atof (optarg);
      rm_set = true;
      break;

    case 'S': nsigma = atof(optarg); break;

    case 't':
      maxmthd = true;
      auto_maxmthd = true;
      break;

    case 'T':
      if (sscanf(optarg, "%f", &threshold) != 1) {
	cerr << "That is not a valid cut-off!" << endl;
	return -1;
      }
      break;

    case 'J':

      refine = true;          // <delta PA> two halves of band
      auto_maxrm = 2000.0;
      auto_minrm = 100.0;
      auto_maxrm_dm = 6.0;    // max RM = 6.0 * DM
      auto_step_rad = 1.0;    // compute step such that one radian

      log_results = true;

      cpg_device = "rmsearch.ps/cps";
      display = true;

      break;

    case 'u':
      auto_maxrm = atoi (optarg);
      break;

    case 'U':
      auto_maxrm_dm = atof (optarg);
      break;

    case 'v':
      verbose = true;
      break;

    case 'V':
      verbose = true;
      Pulsar::Archive::set_verbosity(3);
      break;

    case 'z': 
      {
	vector<string> words = stringdecimate(optarg," \t");
	for( unsigned i=0; i<words.size(); i++)
	  zap_chans.push_back( fromstring<unsigned>(words[i]) );
	break;
      }


    default:
      cout << "Unrecognised option" << endl;
    }
  }

  // Parse the list of archives

  for (int ai=optind; ai<argc; ai++)
    dirglob (&archives, argv[ai]);

  if (archives.empty()) {
    cerr << "No archives were specified" << endl;
    return -1;
  }

#if HAVE_PGPLOT
  if (display)
    {
      unsigned nx = 1;
      unsigned ny = 1;
      cpgopen(cpg_device.c_str());
      cpgsubp(nx,ny);
    }
#endif

  for (unsigned i = 0; i < archives.size(); i++) try
    {
      Reference::To<Pulsar::Archive> data = get_data(archives[i]);

      if (rm_set)       data->set_rotation_measure (rotation_measure);
      if (fscrunchme)   data -> fscrunch(fscr);
      if (bscrunchme)   data -> bscrunch(bscr);

      if (!include_range.empty())
	parse_indeces (include_bins, include_range, data->get_nbin());

      if (!exclude_range.empty())
	parse_indeces (exclude_bins, exclude_range, data->get_nbin());

      for( unsigned izap=0; izap<zap_chans.size(); izap++)
	for( unsigned iint=0; iint<data->get_nsubint(); iint++)
	  data->get_Integration(iint)->set_weight(zap_chans[izap],0.0);

      if (maxmthd)
	{
	  double best_rm = do_maxmthd (minrm, maxrm, rmsteps, data);

	  data->set_rotation_measure (best_rm);

	  if( verbose )
	    fprintf(stderr,"Completed do_maxmthd and got out best_rm=%f\n",
		    best_rm);

	  if (!refine)
	    continue;
	}

      // This must be done after maxmthd because it re-lables
      // all the channel frequencies.

      if (refine) try
	{
	  if (verbose)
	    cerr << "rmfit: refining rotation measure estimate" << endl;

	  do_refine (data,log_results);
	  continue;
	}
      catch (Error& error)
	{ 
	  cerr << "rmfit: Error during refine" << error << endl; 
	  return -1;
	} 

    }
  catch (Error& error)
    { 
      cerr << error << endl;
      continue;
    }

#if HAVE_PGPLOT
  if (display)
    cpgend ();
#endif

  return 0;
}

// defined in width.C
float width (const Pulsar::Profile* profile, float& error, float pc, float dc);

void rmresult (Pulsar::Archive* archive,
	       const Estimate<double>& rm, unsigned used)
{
  unsigned original_nchan = archive->get_nchan();

  archive->set_rotation_measure( rm.get_value() );
  archive->defaraday();
  archive->fscrunch();
  archive->remove_baseline();
  archive->centre_max_bin();

#if HAVE_PGPLOT

  string dev = archive->get_source () + ".ps/cps";
  cerr << "\nrmresult: PGPLOT to " << dev << endl << endl;
  cpgbeg (0, dev.c_str(), 0, 0);
  cpgslw(2);
  cpgsvp (.1,.9, .1,.9);

  Pulsar::StokesCylindrical plot;

  plot.get_frame()->publication_quality ();
  plot.get_frame()->set_character_height (1.0);

  // phase in degrees, with origin at pulse peak
  plot.get_scale()->set_units (Pulsar::PhaseScale::Degrees);
  plot.get_scale()->set_origin_norm (-0.5);

  // linear polarization with dash-dot-dash-dot
  plot.get_flux()->set_plot_lines ("131");

  // plot the time resolution and flux error box
  plot.get_flux()->set_original_nchan (original_nchan);
  plot.get_flux()->set_plot_error_box (2);

  // zoom so that the on-pulse region occupyies a fraction of the phase axis
  plot.get_flux()->auto_scale_phase (archive->get_Profile(0,0,0), 2.5);
  // increase the buffer to 10% on either side
  plot.get_flux()->get_frame()->get_y_scale()->set_buf_norm (0.1);
  // separate the name and frequency
  plot.get_flux()->get_frame()->get_label_below()->set_left ("$name");
  plot.get_flux()->get_frame()->get_label_below()->set_right ("$freq MHz");

  // plot -90 to 180 in the PA plot
  plot.get_orientation()->set_span (180.0);
  plot.get_orientation()->get_frame()->get_y_scale()->set_range_norm (0, 1.5);

  plot.get_orientation()->set_marker( (Pulsar::AnglePlot::Marker)
				      ( Pulsar::AnglePlot::ErrorTick ) );

  plot.plot (archive);

  cpgend ();

#endif

  FILE* fptr = fopen ("rmfit.out", "a");
  if (!fptr) {
    cerr << "Could not open rmfit.out ";
    perror ("");
  }

  fprintf (stderr, "Setting up PolnProfileStats\n");

  Pulsar::PolnProfileStats stats;
  stats.set_profile( archive->get_Integration(0)->new_PolnProfile(0) );
  unsigned nbin = archive->get_nbin();

  fprintf (stderr, "Writing title\n");

#define FORMAT1 "%10s "
#define FORMAT2 "%10.4f "

  fprintf (fptr, FORMAT1, "NAME");
  fprintf (fptr, FORMAT1, "<I>");
  fprintf (fptr, FORMAT1, "<p>");
  fprintf (fptr, FORMAT1, "<L>");
  fprintf (fptr, FORMAT1, "<V>");
  fprintf (fptr, FORMAT1, "<|V|>");
  fprintf (fptr, FORMAT1, "sigma");
  fprintf (fptr, FORMAT1, "unit");
  fprintf (fptr, FORMAT1, "W50");
  fprintf (fptr, FORMAT1, "W10");
  fprintf (fptr, FORMAT1, "res");
  fprintf (fptr, FORMAT1, "RM");
  fprintf (fptr, FORMAT1, "RM_err");
  fprintf (fptr, FORMAT1, "freq");
  fprintf (fptr, FORMAT1, "bw");
  fprintf (fptr, FORMAT1, "nchan");
  fprintf (fptr, FORMAT1, "nbin");
  fprintf (fptr, FORMAT1, "filename");
  fprintf (fptr, "\n");

  fprintf (fptr, FORMAT1, archive->get_source().c_str());

  Estimate<double> intensity = stats.get_total_intensity () / nbin;
  fprintf (stderr, "flux       %lf mJy\n", intensity.get_value());
  fprintf (stderr, "flux_err   %lf\n\n",   intensity.get_error());


  fprintf (fptr, FORMAT2, intensity.get_value());

  Estimate<double> polarization = stats.get_total_polarized () / nbin;
  fprintf (stderr, "poln       %lf mJy\n", polarization.get_value());
  fprintf (stderr, "poln_err   %lf\n\n",   polarization.get_error());

  fprintf (fptr, FORMAT2, polarization.get_value());

  Estimate<double> linear = stats.get_total_linear () / nbin;
  fprintf (stderr, "linear     %lf mJy\n", linear.get_value());
  fprintf (stderr, "linear_err %lf\n\n",   linear.get_error());

  fprintf (fptr, FORMAT2, linear.get_value());

  Estimate<double> circular     = stats.get_total_circular () / nbin;
  Estimate<double> abs_circular = stats.get_total_abs_circular () / nbin;
  fprintf (stderr, "circ       %lf mJy\n", circular.get_value());
  fprintf (stderr, "abs(circ)  %lf\n",     abs_circular.get_value());
  fprintf (stderr, "circ_err   %lf\n\n",   circular.get_error());

  fprintf (fptr, FORMAT2, circular.get_value());
  fprintf (fptr, FORMAT2, abs_circular.get_value());

  // sigma
  Estimate<double> rms = sqrt( stats.get_baseline_variance(0) );

  fprintf (fptr, FORMAT2, rms.get_value() );

  if (archive->get_scale() == Signal::Jansky)
    fprintf (fptr, FORMAT1, "mJy");
  else
    fprintf (fptr, FORMAT1, "Arb");

  double period = archive->get_Integration(0)->get_folding_period () * 1e3;

  Estimate<double> width;
  float width_err;
  width.val = ::width (archive->get_Profile(0,0,0), width_err, 50.0, 0.15);
  width.var = width_err * width_err;
  width *= period;

  fprintf (stderr, "W50        %lf ms\n", width.get_value());
  fprintf (stderr, "W50_err    %lf \n\n", width.get_error());

  // degrees written to file
  width *= 360.0/period;
  fprintf (fptr, FORMAT2, width.get_value());

  width.val = ::width (archive->get_Profile(0,0,0), width_err, 10.0, 0.15);
  width.var = width_err * width_err;

  width *= period;

  fprintf (stderr, "W10        %lf ms\n", width.get_value());
  fprintf (stderr, "W10_err    %lf \n\n", width.get_error());

  // degrees written to file
  width *= 360.0/period;
  fprintf (fptr, FORMAT2, width.get_value());

#if HAVE_PGPLOT
  // resolution in degrees
  float res_deg = plot.get_flux()->get_phase_error(archive) * 360.0;
  fprintf (fptr, FORMAT2, res_deg);
#endif

  fprintf (fptr, FORMAT2, rm.get_value());

  fprintf (fptr, FORMAT2, rm.get_error());

  fprintf (fptr, FORMAT2, archive->get_centre_frequency());

  fprintf (fptr, FORMAT2, fabs(archive->get_bandwidth()));

  fprintf (fptr, "%10u ", original_nchan);

  fprintf (fptr, "%5u/%u ", used, archive->get_nbin());

  fprintf (fptr, "%s \n", archive->get_filename().c_str());

  fclose (fptr);

  cerr << "\nrmresult: data written to rmfit.out" << endl;

  string filename = archive->get_source() + ".data";
  fptr = fopen (filename.c_str(), "w");
  if (!fptr) {
    cerr << "Could not open " << filename;
    perror ("");
  }

  fprintf (fptr, "# Source:%s Freq:%f BW:%f RM:%f nbin:%u\n",
	   archive->get_source().c_str(),
	   archive->get_centre_frequency(),
	   archive->get_bandwidth(),
	   archive->get_rotation_measure(),
	   archive->get_nbin());

  Reference::To<Pulsar::PolnProfile> profile;
  profile = archive->get_Integration(0)->new_PolnProfile(0);

  Pulsar::Profile L;
  vector< Estimate<double> > PA;

  profile->get_linear( &L );

  profile->get_orientation( PA, 0 );

  fprintf (fptr, "# bin I Q U V L PA\n");
  for (unsigned ibin=0; ibin < archive->get_nbin(); ibin++)
    fprintf (fptr, "%u %f %f %f %f %f %lf\n", ibin,
	     profile->get_Profile(0)->get_amps()[ibin],
	     profile->get_Profile(1)->get_amps()[ibin],
	     profile->get_Profile(2)->get_amps()[ibin],
	     profile->get_Profile(3)->get_amps()[ibin],
	     L.get_amps()[ibin],
	     PA[ibin].get_value());

  fclose (fptr);

  cerr << "\nrmresult: data written to " << filename << endl;

}

Reference::To<Pulsar::Archive> get_data(string filename)
{
  Reference::To<Pulsar::Archive> data = Pulsar::Archive::load(filename);

  // data -> set_filename( "Archive: " + filename );

  data -> convert_state(Signal::Stokes);

  data -> dedisperse();
  data -> tscrunch();

  data -> remove_baseline();
  data -> defaraday();

  return data;
}

double do_maxmthd (double minrm, double maxrm, unsigned rmsteps,
		   Reference::To<Pulsar::Archive> data)
{
  if (auto_maxmthd)
    {
      // compute the maximum (and minimum) measurable rotation measure ...

      // frequency in Hz
      double nu = data->get_centre_frequency () * 1e6;
      // wavelength in metres
      double lambda = Pulsar::speed_of_light / nu;
      // channel bandwidth in Hz
      double dnu = fabs(data->get_bandwidth()) * 1e6 / data->get_nchan();

      double RM_max = 0.5 / (lambda*lambda) * nu / dnu;

      rmsteps = 2 * data->get_nchan();

      double step_size = 2 * RM_max / rmsteps;

      cerr << "rmfit: default auto maximum RM=" << RM_max << " steps=" << rmsteps
	   << " (step size=" << step_size << ")" << endl;

      if (auto_maxrm_dm > 0) {
	double dm = data->get_dispersion_measure();
	float dm_auto_maxrm = auto_maxrm_dm * dm;
	cerr << "rmfit: maximum RM = (" << auto_maxrm_dm << " * DM=" << dm
	     << ") = " << dm_auto_maxrm << endl;
	auto_maxrm = std::min (auto_maxrm, dm_auto_maxrm);
      }

      bool override = false;

      if (auto_maxrm > 0 && RM_max > auto_maxrm) {
	cerr << "rmfit: default auto maximum RM over limit = " << auto_maxrm 
	     << endl;
	RM_max = auto_maxrm;
	override = true; 
      }

      if (auto_minrm > 0 && RM_max < auto_minrm) {
	cerr << "rmfit: default auto maximum RM under limit = " << auto_minrm 
	     << endl;
	RM_max = auto_minrm;
	override = true;
      }

      if (auto_step_rad > 0)
	{
	  double centre_0 = nu - data->get_nchan() * dnu;
	  double lambda_0 = Pulsar::speed_of_light / centre_0;

	  double centre_1 = nu + data->get_nchan() * dnu;
	  double lambda_1 = Pulsar::speed_of_light / centre_1;

	  // cerr << "lambda_0=" << lambda_0 << " lambda_1=" << lambda_1 << endl;

	  step_size = auto_step_rad / (lambda_0*lambda_0 - lambda_1*lambda_1);

	  cerr << "rmfit: setting step size = " << step_size 
	       << " (" << auto_step_rad << " radians between two halves of band)"
	       << endl;

	  override = true;
	}

      if (override)
	{
	  rmsteps = unsigned( 2 * RM_max / step_size );
	  cerr << "rmfit: override maximum RM=" << RM_max
	       << " steps=" << rmsteps << endl;
	}

      if (auto_minsteps > 0 && rmsteps < auto_minsteps)
	{
	  rmsteps = auto_minsteps;
	  cerr << "rmfit: override steps=" << auto_minsteps 
	       << " (step size=" << 2*RM_max/rmsteps << ")" << endl;
	}

      minrm = -RM_max;
      maxrm = RM_max;
    }

  vector<float> fluxes (rmsteps);
  vector<float> rms (rmsteps);
  vector<float> err (rmsteps);  

  float rmstepsize = (maxrm-minrm)/float(rmsteps-1);

  Reference::To<Pulsar::Archive> backup = data->clone();

  for (unsigned step=0; step < rmsteps; step++)
    {
      double rm = minrm + step * rmstepsize;

      /*
	WvS, 12 July 2006: Note that Archive::defaraday can handle the
	case that the data have already been corrected for a certain
	rotation measure.

	Wvs, 26 September 2007: Then again, perhaps round-off error
	can build up over many iterations.
      */

      data->set_rotation_measure( rm );
      data->defaraday ();

      Reference::To<Pulsar::Archive> useful = data->clone();
      useful->fscrunch();
      useful->remove_baseline();

      Pulsar::PolnProfileStats stats;
      stats.set_profile( useful->get_Integration(0)->new_PolnProfile(0) );
      Estimate<float> rval = stats.get_total_linear ();

      fluxes[step] = rval.get_value();
      err[step] = rval.get_error();
      rms[step] = rm;

      data = backup->clone();
    }

#if HAVE_PGPLOT

  float ymin = fluxes[0];
  float ymax = fluxes[0];

  if (display)
  {
    cpg_next();

    for( unsigned i=0; i<rms.size(); i++){
      if( fluxes[i]-err[i] < ymin )
	ymin = fluxes[i]-err[i];
      if( fluxes[i]+err[i] > ymax )
	ymax = fluxes[i]+err[i];
    }

    float yrange = ymax-ymin;
    ymin -= 0.1*yrange;
    ymax += 0.1*yrange;

    float buffer = fabs((maxrm-minrm)/float(rmsteps));
    cpgswin(minrm-buffer, maxrm+buffer, ymin, ymax);
    cpgbox("BCINTS", 0.0, 0, "BCINTS", 0.0, 0);
    cpglab("Rotation Measure", "Polarised Flux", "");
    for (unsigned k = 0; k < rms.size(); k++) {
      cpgpt1(rms[k], fluxes[k], 0);
      cpgerr1(6, rms[k], fluxes[k], err[k], 1.0);
      cpgsci(1);
    }

  }

#endif

  unsigned index = max_element(fluxes.begin(), fluxes.end()) - fluxes.begin();
  float max = fluxes[index];

  // fit data only to the first set of minima
  unsigned index_min = 0;
  for (index_min = index -1; index_min > 1; index_min --)
    if (fluxes[index_min] > fluxes[index_min+1]) {
      index_min ++;
      break;
    }

  unsigned index_max = index;
  for (index_max = index +1; index_max < rmsteps; index_max ++)
    if (fluxes[index_max] > fluxes[index_max-1]) {
      index_max --;
      break;
    }

  double bestrm = rms[index];

  // Fit a Gaussian curve to the points      
  float threshold = 0.001;

  MEAL::Gaussian gm;

  gm.set_centre (bestrm);
  gm.set_width  (rms[index_max] - rms[index_min]);
  gm.set_height (max);

  cerr << "initial peak index=" << index 
       << "  (" << index_min << "->" << index_max << ") or\n"
       << "  centre=" << bestrm << "  width=" << gm.get_width() 
       << "  height=" << gm.get_height() << endl;

  MEAL::Axis<double> argument;
  gm.set_argument (0, &argument);

  vector< MEAL::Axis<double>::Value > data_x; // x-ordinate of data
  vector< Estimate<double> > data_y; // y-ordinate of data with error

  for (unsigned j = index_min; j < index_max; j++) {
    data_x.push_back ( argument.get_Value(rms[j]) );
    data_y.push_back( Estimate<double>(fluxes[j],err[j]) );
  }

  if (data_x.size() == 0) {
    cerr << "WARNING: zero width; returning best RM = " << bestrm << endl;
    return bestrm;
  }

  MEAL::LevenbergMarquardt<double> fit;

  try {

    float chisq = fit.init (data_x, data_y, gm);
    if (verbose)
      cerr << "initial chisq = " << chisq << endl;

    unsigned iter = 1;
    unsigned not_improving = 0;
    while (not_improving < 25) {
      if (verbose)
	cerr << "iteration " << iter << endl;

      float nchisq = fit.iter (data_x, data_y, gm);

      if (verbose)
	cerr << "     chisq = " << nchisq << endl;

      if (nchisq < chisq) {
	float diffchisq = chisq - nchisq;
	chisq = nchisq;
	not_improving = 0;
	if (diffchisq/chisq < threshold && diffchisq > 0) {
	  if (verbose)
	    cerr << "No big diff in chisq = " << diffchisq << endl;
	  break;
	}
      }
      else
	not_improving ++;

      iter ++;
    }

    double free_parms = data_x.size() + gm.get_nparam();

    cerr << "Chi-squared = " << chisq << " / " << free_parms << " = "
	 << chisq / free_parms << endl;

    cerr << "Width="<< gm.get_width() <<" Height="<< gm.get_height() << endl;

    bestrm = gm.get_centre();

    // Compute the error based on how far you have to move the
    // centre of the Gaussian before the two-sigma

    float aim = chisq + 4.0;
    float cst = chisq;

    double raw = gm.get_centre();
    double itr = 0.0;

    //  fprintf(stderr,"Got raw=%f cst=%f chisq=%f aim=%f\n",raw,cst,chisq,aim);
    double maxitr = gm.get_width() / 2.0;
    while (cst < aim && itr < maxitr) {
      gm.set_centre(raw + itr);
      cst = fit.init(data_x, data_y, gm);
      itr += 0.01;
    }

    cout << "Best RM is: " << bestrm << " +/- " << itr << endl;

    best_search_rm.set_value ( bestrm );
    best_search_rm.set_error ( itr );

  }
  catch (Error& error) {
    cerr << "rmfit: failed to fit for best RM.  Using peak value." << endl;
    cout << "Best RM is: " << bestrm << endl;
  }

#if HAVE_PGPLOT

  if (display) {

    cpgsci(2);
    cpgsls(2);
    cpgmove(bestrm, ymax);
    cpgdraw(bestrm, ymin);
    cpgsci(1);
    cpgsls(1);

    gm.set_centre(bestrm);
    gm.set_abscissa(rms[0]);
    cpgmove(rms[0],gm.evaluate());
    cpgsci(2);

    for (unsigned k = 1; k < rms.size(); k++) {
      gm.set_abscissa(rms[k]);
      cpgdraw(rms[k],gm.evaluate());
    }

    cpgsci(1);
  }

#endif

  return bestrm;
}

void do_refine (Reference::To<Pulsar::Archive> data, bool log_results)
{
  Pulsar::DeltaRM delta_rm;

  if (refine_threshold)
    delta_rm.set_threshold (refine_threshold);

  delta_rm.set_include (include_bins);
  delta_rm.set_exclude (exclude_bins);

  bool converged = false;
  unsigned iterations = 0;

  Estimate<double> old_rm;
  Estimate<double> new_rm;

  while (!converged) {

    if (iterations > max_iterations) {

      cerr << "rmfit: maximum iterations (" << max_iterations << ") exceeded"
	   << endl;

      cerr << "new=" << new_rm << " old=" << old_rm << endl;

      double diff_new = fabs( best_search_rm.get_value() - new_rm.get_value() );
      double diff_old = fabs( best_search_rm.get_value() - old_rm.get_value() );

      if ( diff_old < diff_new )
	new_rm = old_rm;

      cerr << "rmfit: best search RM=" << best_search_rm 
	   << " using closest RM=" << new_rm << endl;

      if (log_results)
	rmresult (data, new_rm, data->get_nbin());

      return;

    }

    old_rm = new_rm;

    try {
      delta_rm.set_data (data->clone());
      delta_rm.refine ();
    }
    catch (Error& error) {
      cerr << "\nrmfit: DeltaRM::refine failed \n\t" << error.get_message() << endl;
      cerr << "rmfit: using best search RM=" << best_search_rm << endl;
      if (log_results)
	rmresult (data, best_search_rm, data->get_nbin());
      return;
    }

    cerr << endl;

    new_rm = delta_rm.get_rotation_measure();
    double new_RM = new_rm.get_value();
    double err_RM = new_rm.get_error();
    double old_RM = data->get_rotation_measure();

    converged = fabs (old_RM - new_RM) <= err_RM;
    iterations ++;

    if (iterations > max_iterations / 2) {
      new_RM = (new_RM + old_RM) / 2.0;
      cerr << "Getting old ... try mean=" << new_RM << endl;
    }

    data->set_rotation_measure (new_RM);

  }

  cerr << "rmfit: converged in " << iterations << " iterations" << endl;
  cerr << "       final rotation measure = " 
       << delta_rm.get_rotation_measure() << endl;

  if (log_results)
    rmresult (data, delta_rm.get_rotation_measure(), delta_rm.get_used_bins());
}

#if HAVE_PGPLOT

void cpg_next ()
{
  cpgsch (1.2);
  cpgsls (1);
  cpgslw (1);
  cpgsci (1);
  cpgsvp (0.1, 0.9, 0.2, 0.8);
  cpgpage ();
}

#endif

