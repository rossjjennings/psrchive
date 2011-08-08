/***************************************************************************
 *
 *   Copyright (C) 2011 by Stefan Oslowski, Tim Dolley and Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// Singla Pulse Analysis - reimplementation and extension of Tim Dolley and Aidan Hotan's spa as an application along with new features.
// Parts of the code corresponding to the functionality of spa are taken directly out of the spa.C, hence I included Tim and Aidan in the note above.
// TODO scan_pulses could be extended to fit the pulses with Gaussian / von Mises / Triangle / etc and use this to determine pulse width
// TODO range setting for flux / PA / polarisation degree histograms

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/PlotOptions.h"
#include "Pulsar/ProfileWeightFunction.h"

#include "MEAL/LevenbergMarquardt.h"
#include "MEAL/Gaussian.h"
#include "MEAL/Axis.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/ProfileStats.h"
#include "Pulsar/PhaseWeight.h"

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>

#include <cpgplot.h>

#include "strutil.h"
#include "dirutil.h"
#include "Error.h"
#include "Warning.h"

#include <gsl/gsl_histogram.h>
#include <gsl/gsl_errno.h>

using namespace std;
using namespace Pulsar;

static Warning warn;

class psrspa : public Application
{
public:
  //! Default constructor:
  psrspa ();
  //! Process the given archive:
  void process ( Archive* );

  //! Final processing:
  void finalize ();

  //! Initial setup;
  void setup ();

  typedef struct pulse
  {
    string file;           // filename
    int    intg;           // integration number
    float  flx;            // pulse flux
    float  phs;            // phase
    float  err;            // error in flux
  } pulse;

  typedef struct hbin
  {
    float x;
    float y;
    float e;
  } hbin;

  typedef struct identifier
  {
    string file;
    unsigned integration;
    unsigned phase_min;
    unsigned phase_max;
    unsigned bscrunch_factor;
    unsigned max_amp_bin;
    float max_amp_in_range;
    double b_sigma; 
  } identifier;

  //////////////////////////////////////////////////////////////////////////////
  // FUNCTION PROTOTYPES

  //! Find and store the flux and phase of each pulse in the file to the data vector.

  // END FUNCTION PROTOTYPES
  //////////////////////////////////////////////////////////////////////////////
protected:
  //! add program options:
  void add_options ( CommandLine::Menu& );

  unsigned nbin;
  bool  gaussian;
  float factor;
  bool  log;
  bool  shownoise;
  int   method;
  unsigned bins;
  float cphs;
  float dcyc;
  float norm;

  bool perform_scan;
  bool perform_find_giants;

  vector<pulse>  pulses;
  vector<pulse>  noise;
  vector<pulse>  pgiants;
  vector<pulse>  ngiants;
  vector<hbin>   pdata;
  vector<hbin>   ndata;

  //! phase resolved histograms
  unsigned hist_count;
  vector<float> phase_range;
  vector<gsl_histogram*> h_polar_degree_vec;
  vector<gsl_histogram*> h_polar_angle_vec;
  vector<gsl_histogram*> h_flux_pr_vec; // pr = phase resolved as opposed to the one calculated in the "traditional" spa

  unsigned polar_degree_bins;
  unsigned polar_angle_bins;
  unsigned flux_pr_bins;

  //! maximal amplitude (and the corresponding bin) in given range:
  vector < identifier > max_amp_info;

  //! Vector of bins corresponding to given histogram
  vector<unsigned> h_bin;

  bool create_flux;
  bool create_polar_degree;
  bool create_polar_angle;

  bool find_max_amp_in_range;
  bool perform_bscrunch_loop;
  unsigned max_bscrunch;
  unsigned current_bscrunch;

  bool not_initialised; 

  int scan_pulses(Reference::To<Pulsar::Archive> arch, vector<pulse>& data, 
		  int method, float cphs, float dcyc);

  //! Finds the mean flux from the data vector. Any flux greater than <factor> times the mean is considered a giant and is copied to the giants array.
  float find_giants(vector<pulse>& data, vector<pulse>& giants, 
		  float factor,  float norm, float offs);

  void  prob_hist(vector<pulse>& data, vector<hbin>& hist, unsigned nbin,
		  float min = -99.0, float max = 99.0);

  void  fit_gauss(vector<hbin>& data, MEAL::Gaussian& gm);

  //! Displays info about giant pulses on screen
  void  display_giants(vector<pulse>& giants);

  void choose_phase ( float );
  void choose_range ( string );
  void choose_bscrunch ( unsigned );

  //! ProfileStats
  ProfileStats stats;
  //! baseline sigma
  double b_sigma;

  //! initialise phase resolved histograms
  void initialise_histograms () ;
  //! Create phase resolved histograms
  void create_histograms ( Reference::To<Archive> );

  //! convenience function to perform all of the "traditional" functions of spa
  void traditional_spa ();

  //! output control
  string prefix;
  string ext;
  string path;
  bool binary_output;

  //! Handle output
  void write_histograms ();

  //! The algorithm used to find pulses
  Reference::To<Pulsar::ProfileWeightFunction> finder;
  //! Set and configure the algorithm used to find pulses
  void set_finder (const std::string& name);
  //! Use the finder to list pulse information
  void matched_finder (const Archive*);
  void matched_report (const PhaseWeight& weight, const Profile& profile);
};


// constructor
psrspa::psrspa ()
	: Application ( "psrspa", "Single Pulse Analysis" )
{
  add ( new StandardOptions );
  //add ( new PlotOptions );

  gaussian   = false;
  factor     = 5.0;
  log        = false;
  shownoise  = false;
  method     = 0;
  polar_degree_bins = polar_angle_bins = flux_pr_bins = bins = 30;
  cphs       = 0.0;
  dcyc       = 0.0;
  norm       = 0.0;
  nbin = 0;

  create_flux = false;
  create_polar_degree = false;
  create_polar_angle = false;

  hist_count = 0;

  find_max_amp_in_range = false;
  perform_bscrunch_loop = false;

  max_bscrunch = 1;
  current_bscrunch = 1;
  
  not_initialised = true;

  perform_scan = false;
  perform_find_giants = false;

  prefix = "psrspa";
  ext = "dat";
  path = "./";
  binary_output = false;

  b_sigma = -1.0;
}

void psrspa::setup ()
{
  if ( bins != 30 )
    polar_degree_bins = polar_angle_bins = flux_pr_bins = bins ;
  if ( perform_find_giants || gaussian )
    perform_scan = true;
  if ( ! perform_scan && ! perform_find_giants && ! create_flux && ! create_polar_degree && ! create_polar_angle && ! find_max_amp_in_range )
    throw Error ( InvalidState, "psrspa::setup", "at least one of -rs, -rg, -hf, -hd, -ha or -fm needs to be used" );
}

void psrspa::process ( Archive* arch )
{
  if ( nbin == 0 )
    nbin = arch->get_nbin();
  else if ( nbin != arch->get_nbin() )
    throw Error ( InvalidParam, "psrspa::process", "number of bins mismatach" );
  if ( ( create_flux || create_polar_degree || create_polar_angle || find_max_amp_in_range ) && ( not_initialised ) )
    initialise_histograms ();
  arch->fscrunch();        // remove frequency and polarisation resolution
  if ( !create_polar_degree && !create_polar_angle )
    arch->pscrunch();
  arch->remove_baseline(); // Remove the baseline level

  //this has to precede create_histograms as the latter can modify the archives (bscrunch them). This can be easily fixed by cloning the archive, if that would prove necessary
  if ( perform_scan )
    scan_pulses(arch, pulses, method, cphs, dcyc);
  if (finder)
  {
    matched_finder (arch);
    return;
  }

  if (shownoise)
    scan_pulses(arch, noise, 2, arch->find_min_phase(dcyc), dcyc);
  if ( create_flux || create_polar_degree || create_polar_angle || find_max_amp_in_range )
  {
    create_histograms ( arch );
  }
}

void psrspa::matched_finder ( const Archive* arch )
{
  Reference::To<Profile> profile = arch->get_Profile(0,0,0)->clone();
  
  while (profile->get_nbin() > 256)
  {
    finder->set_Profile( profile );  // might finder optimize on &profile?

    PhaseWeight weight;
    finder->get_weight( &weight );

    matched_report (weight, *profile);

    profile->bscrunch (2);
  }
}

void psrspa::matched_report (const PhaseWeight& weight, const Profile& profile)
{
  const unsigned nbin = weight.get_nbin();
  assert (nbin == profile.get_nbin());

  unsigned ibin = 0;

  while (ibin < nbin)
  {
    // find the first on-pulse phase bin
    while (ibin < nbin && !weight[ibin]) ibin++;

    if (ibin == nbin)
      break;

    // start of an on-pulse region ... sum up the flux in this region
    unsigned istart = ibin;
    double flux = 0;
    while (weight[ibin] && ibin < nbin)
    {
      flux += profile.get_amps()[ibin];
      ibin++;
    }

    unsigned iend = ibin-1;

    // mid-point of region defines phase
    double phase = 0.5*(istart + iend);

    // end-points of region define width
    double width = iend - istart;

    cout << "nbin=" << nbin << " phase=" << phase << " -> " << phase/nbin
         << " flux=" << flux << " width=" << width << " -> " << width/nbin
         << endl;
  }
}

void psrspa::finalize ()
{
  if ( perform_scan || perform_find_giants )
    traditional_spa ();
  if ( create_flux || create_polar_degree || create_polar_angle || find_max_amp_in_range )
    write_histograms ();
}

void psrspa::traditional_spa ()
{
  if ( verbose )
    cerr << "psrspa::traditional_spa entered" << endl;
  float offset = 2.0;

  if (shownoise) {
    offset = noise[0].flx;
    for (unsigned i = 1; i < noise.size(); i++)
      if (noise[i].flx < offset) offset = noise[i].flx;
    offset = fabs(offset);
    offset /= norm;
  }

  float threshold = find_giants(pulses, pgiants, factor, norm, offset);
  cout << "Detection threshold is roughly " << threshold << endl;

  display_giants(pgiants);

  prob_hist(pulses, pdata, bins);


  if ( verbose )
    cerr << "psrspa::traditional_spa calculating submin and submax " << endl;

  float submin = pdata[0].x;
  float submax = pdata[0].x;
  for (unsigned i = 1; i < pdata.size(); i++) {
    if (pdata[i].x > submax) submax = pdata[i].x;
    if (pdata[i].x < submin) submin = pdata[i].x;
  }

  if (shownoise) {
    if ( verbose )
      cerr << "psrspa::traditional_spa executing code for the background noise calculations" << endl;
    find_giants(noise, ngiants, factor, norm, offset);
    prob_hist(noise, ndata, bins, submin, submax);
  }

  unsigned useful = 0;
  if (log) {
    if ( verbose )
      cerr << "psrspa::traditional_spa finding data suitable for a logarithmic plot" << endl;
    vector<hbin>::iterator it = pdata.begin();
    while (it != pdata.end()) {
      if (pdata[useful].x < 0) {
	pdata.erase(it);
	continue;
      }
      else {
	pdata[useful].x = logf(pdata[useful].x);
	it++;
	useful++;
      }
    }
    threshold = logf(threshold);
  }

  if ( verbose )
    cerr << "psrspa::traditional_spa searching for extremes in the data" << endl;
  float xmin, xmax, ymin, ymax;

  xmin = xmax = pdata[0].x;
  ymin = ymax = pdata[0].y;

  // Find the extremes of the data set

  for (unsigned i = 1; i < pdata.size(); i++) {
    if (pdata[i].x > xmax) xmax = pdata[i].x;
    if (pdata[i].x < xmin) xmin = pdata[i].x;
    if (pdata[i].y > ymax) ymax = pdata[i].y;
    if (pdata[i].y < ymin) ymin = pdata[i].y;
  }


  // Plot pulse intensity probability distribution
  if ( verbose )
    cerr << "psrspa::traditional_spa plotting pulse intensity probability distribution" << endl;
  cpgopen("?");
  cpgsvp(0.1,0.9,0.15,0.9);
  cpgsci(7);
  cpgsch(1.3);

  cpgswin(xmin-(xmin/100.0), xmax+(xmax/100.0),
		  logf(ymin-(ymin/2.0)), logf(ymax+(ymax/2.0)));

  cpgbox("BCNST", 0.0, 0, "BCNST", 0.0, 0);
  cpgsci(8);
  if (log)
    cpglab("Log (Normalised Pulse Flux)", "Log (P(I))", "");
  else 
    cpglab("Normalised Pulse Flux", "Log (P(I))", "");
  cpgsci(5);


  // Plot the pulse detection threshold
  if ( verbose )
    cerr << "psrspa::traditional_spa plotting the pulse detection threshold" << endl;
  cpgsls(3);
  cpgsci(3);
  cpgmove(threshold, logf(ymin-(ymin/2.0)));
  cpgdraw(threshold, logf(ymax+(ymax/2.0)));
  cpgsls(1);

  // Plot points and associated error bars

  if ( verbose )
    cerr << "psrspa::traditional_spa plotting points" << endl;
  for (unsigned i = 0; i < pdata.size(); i++) {
    cpgpt1(pdata[i].x, logf(pdata[i].y), 4);
    cpgerr1(6, pdata[i].x, logf(pdata[i].y), pdata[i].e/pdata[i].y, 1.0);
  }

  if (gaussian) {
    if ( verbose )
      cerr << "psrspa::traditional_spa fitting Gaussian model to the probability distribution" << endl;
    MEAL::Gaussian gm1;

    fit_gauss(pdata, gm1);

    gm1.set_abscissa(xmin);
    cpgmove(xmin,logf(gm1.evaluate()));

    cpgsci(2);
    cpgsls(2); 

    float xval = 0.0;

    for (unsigned i = 1; i < 1000; i++) {
      xval = xmin+((xmax-xmin)/1000.0*i);
      gm1.set_abscissa(xval);
      cpgdraw(xval,logf(gm1.evaluate()));
    }

    if (shownoise) {

      MEAL::Gaussian ngm;

      fit_gauss(ndata, ngm);

      ngm.set_abscissa(xmin);
      cpgmove(xmin,logf(ngm.evaluate()));

      cpgsci(5);
      cpgsls(4); 

      for (unsigned i = 1; i < 1000; i++) {
	xval = xmin + ((xmax-xmin)/250.0*i);
	ngm.set_abscissa(xval);
	if (log)
	  cpgdraw(logf(xval),logf(ngm.evaluate()));
	else
	  cpgdraw(xval,logf(ngm.evaluate()));
      }
    }
  }

  cpgclos();

  // Free any used memory
  if ( verbose )
    cerr << "psrspa::traditional_spa freeing up memory" << endl;
  pulses.clear();
  noise.clear();
  pgiants.clear();
  ngiants.clear();
  pdata.clear();
  ndata.clear();

  fflush(stdout);

  if ( verbose )
    cerr << "psrspa::traditional_spa finished" << endl;
}

void psrspa::set_finder (const std::string& name)
{
  finder = ProfileWeightFunction::factory (name);
}

void psrspa::choose_phase ( float _cphs )
{
  cphs = _cphs;
  if ( cphs > 0.0 && cphs < 1.0 )
  {
    method = 2;
  }
  else
  {
    method = 1;
  }
}

void psrspa::choose_range ( string _range )
{
  if ( verbose )
    cerr << "psrspa::chose_range parsing " << _range << " as a list of phase ranges" << endl;
  for ( string sub = stringtok ( _range, "," ); !sub.empty(); sub = stringtok ( _range, "," ) )
  {
    phase_range.push_back(fromstring<float>(sub));
  }
  if ( phase_range.size()%2 != 0 )
    throw Error ( InvalidParam, "psrspa::chose_range", "Minimum and maximum needs to be provided" );
}


int main (int argc, char** argv)
{
    psrspa program;
      return program.main (argc, argv);
}

void psrspa::add_options ( CommandLine::Menu& menu )
{
  CommandLine::Argument* arg;

  menu.add ( "" );
  menu.add ( "Common options" );

  arg = menu.add ( log, 'l' );
  arg->set_help ( "Use a logarithmic flux scale" );

  arg = menu.add ( bins, 'b', "bins" );
  arg->set_help ( "Set the number of all bins" );

  menu.add ( "" );
  menu.add ( "Scan pulses and find giants - options:" );

  arg = menu.add ( this, &psrspa::set_finder, "use");
  arg->set_help ( "Set/configure the algorithm used to find pulses" );    
  arg->set_long_help ( "This is a different implementation of the pulse searching" );

  arg = menu.add ( perform_scan, "rs" );
  arg->set_help ( "perform the scan for pulses, as in spa" );

  arg = menu.add ( perform_find_giants, "rg" );
  arg->set_help ( "perform the search for giants, as in spa"
	       "\t\t\t (this implies -rs)" );

  arg = menu.add ( factor, 'g', "factor" );
  arg->set_help ( "Show giant pulses with flux = <factor>*(norm flux)" );

  arg = menu.add ( this, &psrspa::choose_phase, 'p', "phase" );
  arg->set_help ( "Use peak flux, calculated at phase = <phase>\n"
	       "\t\t\t (enter 0.0 to use the overall peak flux)" );

  arg = menu.add ( dcyc, 'w', "width" );
  arg->set_help ( "Use flux summed over the given phase width\n"
		  "\t\t\t (only with the -p flag)" );

  arg = menu.add ( norm, 'n', "norm" );
  arg->set_help ( "Force the use of a specific normalisation factor" );

  arg = menu.add ( gaussian, 'G' );
  arg->set_help ( "Fit a Gaussian model to the probability distribution"
		  "\t\t\t (this implies -rs)");

  arg = menu.add ( shownoise, 's' );
  arg->set_help ( "Show the best-fit model for the background noise" );

  menu.add ( "" );
  menu.add ( "Phased resolved histograms" );

  arg = menu.add ( this, &psrspa::choose_range, "pr","min,max(,min,max...)" );
  arg->set_help ( "Specify phase range (in turns) for the phase resolved algorithms and max amplitude algorithm" );
  arg->set_long_help ( "Multiple ranges can be provided, e.g., '0.0,0.1,0.3,0.35' will create two ranges 0.0-0.1 and 0.3-0.35" );

  arg = menu.add ( create_flux, "hf");
  arg->set_help ( "Create phase resolved histogram of flux" );

  arg = menu.add ( create_polar_degree, "hd");
  arg->set_help ( "Create phase resolved histogram of polarisation degree" );

  arg = menu.add ( create_polar_angle, "ha");
  arg->set_help ( "Create phase resolved histogram of polarisation angle" );

  arg = menu.add ( find_max_amp_in_range, "fm");
  arg->set_help ( "Find maximum amplitude in the given phase range as provided by -pr" );

  arg = menu.add ( flux_pr_bins, "hfb", "bins" );
  arg->set_help ( "Set the number of phase resolved flux bins" );

  arg = menu.add ( polar_degree_bins, "hdb", "bins" );
  arg->set_help ( "Set the number of polarisation degree bins" );

  arg = menu.add ( polar_angle_bins, "hab", "bins" );
  arg->set_help ( "Set the number of polarisation angle bins" );

  arg = menu.add ( this, &psrspa::choose_bscrunch, "bl", "max_bscrunch" );
  arg->set_help ( "When searching for maximum amplitude in the given phase range, perform calculations\n"
		  "also for bscrunched profiles, with the given maximum bscrunch factor" );

  menu.add ( "" );
  menu.add ( "Output of the histograms" );

  arg = menu.add ( prefix, "pfix","prefix" );
  arg->set_help ( "Write files with this prefix" );

  arg = menu.add ( ext, 'e', "extension" );
  arg->set_help ( "Write files with this extension" );
  
  arg = menu.add ( path, 'u', "path" );
  arg->set_help ( "Write files in this location" ); 

  arg = menu.add ( binary_output, "bo" );
  arg->set_help ( "write the output in binary format" );
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS

int psrspa::scan_pulses(Reference::To<Pulsar::Archive> arch, vector<pulse>& data, 
		int method, float cphs, float dcyc)
{
  if ( verbose )
    cerr << "psrspa::scan_pulses entered" << endl;
  /* Find and store the flux and phase of each pulse in the file
     to the data vector. */
  
  pulse newentry;

  Reference::To<Pulsar::Profile> prof;
  
  newentry.file = arch->get_filename();

  // mean, variance and variance of the mean
  double nm, nv, vm;
  
  int bwid = int(float(nbin) * dcyc);
  int cbin = 0;

  for (unsigned i = 0; i < arch->get_nsubint(); i++) {
    if ( verbose )
      cerr << "psrspa::scan_pulses scanning subint " << i << " of " << newentry.file << endl;

    newentry.intg = i;
    
    prof = arch->get_Profile(i, 0, 0);
     
    prof->stats(prof->find_min_phase(), &nm, &nv, &vm);
    newentry.err = sqrt(nv);

    switch (method) {
      
    case 0: // Method of total flux
     if ( verbose )
       cerr << "psrspa::scan_pulses using the method of the total flux" << endl;

      newentry.flx = prof->sum();
      newentry.phs = prof->find_max_phase();
      break;
      
    case 1: // Method of maximum amplitude
     if ( verbose )
       cerr << "psrspa::scan_pulses using the method of the maximum amplitude" << endl;

      if (dcyc == 0.0) {
	newentry.flx = prof->max();
	newentry.phs  = prof->find_max_phase();
      }
      else {
	cbin = int(prof->find_max_phase(dcyc) * float(nbin));
	newentry.flx = prof->sum(cbin - bwid/2, cbin + bwid/2);
	newentry.phs  = prof->find_max_phase(dcyc);
      }
      break;

    case 2: // User-defined phase centre
     if ( verbose )
       cerr << "psrspa::scan_pulses using the user-definied phase centre" << endl;
      
      cbin = int(float(nbin) * cphs);

      if (dcyc == 0.0) {
	newentry.flx = (prof->get_amps())[cbin];
	newentry.phs = cphs;
      }
      else {
	newentry.flx = prof->sum(cbin - bwid/2, cbin + bwid/2);
	newentry.phs = cphs;
      }
      
      break;
      
    default:
      cerr << "psrspa::scan_pulses No phase selection method chosen!" << endl;
    }
    
    data.push_back(newentry);
  }
  
  if ( verbose )
    cerr << "psrspa::scan_pulses finished with " << data.size () << " pulses found" << endl;
  if ( data.size () == 0 )
    throw Error ( InvalidState, "psrspa::scan_pulses", "Found no pulses! exiting");
  return data.size();
}

//////////////////////////////////////////////////////////////////////////////

float psrspa::find_giants(vector<pulse>& data, vector<pulse>& giants, 
		  float factor, float norm, float offs)
{
  if ( verbose )
    cerr << "psrspa::find_giants entered" << endl;
  /* Finds the mean flux from the data vector. Any flux greater than
     <factor> times the mean is considered a giant and is copied to
     the giants array. */
  
  float mean1 = 0.0;
  float mean2 = 0.0;
  float thres = 0.0;

  // First, calculate mean flux:
  
  for (unsigned i = 0; i < data.size(); i++)
    mean1 += data[i].flx;  

  mean1 /= data.size();

  // Then, normalise the flux values (put in form (flux/norm)) 
  // and find the new mean.

  for (unsigned i = 0; i < data.size(); i++)
    {
      if (norm > 0.0) {
	data[i].flx /= norm;
	data[i].flx += offs;
      }
      else {
	data[i].flx /= mean1;
	data[i].flx += offs;
      }
      mean2 += data[i].flx;
    }
  
  mean2 /= data.size();
  
  // Finally, find the giants and push onto the new vector
  
  giants.clear();
  
  for (unsigned i = 0; i < data.size(); i++) {
    if (data[i].flx > factor)
      giants.push_back(data[i]);

    thres += (data[i].flx - mean2) * (data[i].flx - mean2);
  }
  
  // Call the detection threshold the standard deviation of the
  // fluxes, added to the mean flux. I suppose this assumes most of
  // the pulses are noise-dominated so that the mean represents
  // background emission...

  thres = sqrt(thres/data.size());
  thres += mean2;

  if ( verbose )
    cerr << "psrspa::find_giants finished" << endl;
  return thres;
}

//////////////////////////////////////////////////////////////////////////////

void psrspa::prob_hist(vector<pulse>& data, vector<hbin>& hist, unsigned hist_nbin,
	       float min, float max)
{
  if ( verbose )
    cerr << "psrspa::prob_hist entered" << endl;
  hist.clear();

  if (min == -99.0 && max == 99.0) {
    max = data[0].flx;
    min = data[0].flx;
    
    for(unsigned i = 0; i < data.size(); i++) {
      if (data[i].flx < min) min = data[i].flx;
      if (data[i].flx > max) max = data[i].flx;
    }
  }

  if ( min == max )
  {
    warn << "WARNING psrspa::prob_hist minimal and maximal value in the data are equal; probalby due to too few pulses found ( found " << data.size () << " pulses)\n The range of histogram is being artificially adjusted..." << endl;
    min = 0.9 * min;
    max = 1.1 * max; 
  }
  
  float bwid = (max - min) / float(hist_nbin);
  float a = min, b = min + bwid;

  hbin newbin;

  for(unsigned i = 0; i < hist_nbin; i++) {

    newbin.y = 0;
    
    for(unsigned j = 0; j < data.size(); j++)
      if ((data[j].flx > a) && (data[j].flx < b)) newbin.y++;
    
    if (newbin.y > 0) {
      newbin.e = sqrt(newbin.y); // set error due to counting statistics
      newbin.x = a + bwid/2.0;
      hist.push_back(newbin);
    }
    
    a += bwid;
    b += bwid;
  }
  
  // Convert to a discrete probability distribution
  for (unsigned i = 0; i < hist.size(); i++) {
    hist[i].y /= data.size();
    hist[i].e /= data.size();
  }
  if ( verbose )
    cerr << "psrspa::prob_hist finished" << endl;
}

//////////////////////////////////////////////////////////////////////////////

void psrspa::fit_gauss(vector<hbin>& data, MEAL::Gaussian& gm)
{

  // Instantiate all the machinery required for curve fitting

  float cutoff = 0.001;
  float chisq  = 1.0;
  float nchisq = 1.0;

  unsigned iter = 1;

  unsigned not_improving = 0;

  MEAL::Axis<double> argument;

  vector< MEAL::Axis<double>::Value > data_x;   // x-ordinate of data
  vector< Estimate<double> > data_y; // y-ordinate of data with error

  MEAL::LevenbergMarquardt<double> fit;
  
  double xmin, xmax, ymin, ymax;

  xmin = xmax = data[0].x;
  ymin = ymax = data[0].y;

  // Find the extremes of the data set

  for (unsigned i = 1; i < data.size(); i++) {
    if (data[i].x > xmax) xmax = data[i].x;
    if (data[i].x < xmin) xmin = data[i].x;
    if (data[i].y > ymax) ymax = data[i].y;
    if (data[i].y < ymin) ymin = data[i].y;
  }

  // Fit a Gaussian function to the distribution
  
  gm.set_centre((xmax + xmin)/2.0);
  gm.set_width(xmax - xmin);
  gm.set_height(ymax);
  
  gm.set_argument (0, &argument);
  
  for (unsigned i = 0; i < data.size(); i++) {
    data_x.push_back ( argument.get_Value(data[i].x) );
    data_y.push_back( Estimate<double>(data[i].y, data[i].e) );
  }
  
  chisq = fit.init (data_x, data_y, gm);
  
  iter = 1;
  not_improving = 0;
  while (not_improving < 25) {
    nchisq = fit.iter (data_x, data_y, gm);
    
    if (nchisq < chisq) {
      float diffchisq = chisq - nchisq;
      chisq = nchisq;
      not_improving = 0;
      if (diffchisq/chisq < cutoff && diffchisq > 0) {
	break;
      }
    }
    else
      not_improving ++;
    
    iter ++;
  }
  
  cerr << "Fitted Gaussian has Chi-Squared = " << nchisq << endl;
  
}

//////////////////////////////////////////////////////////////////////////////

void psrspa::display_giants(vector<pulse>& giants)
{
  if ( verbose )
    cerr << "psrspa::display_giants entered" << endl;
  /* Displays info about giant pulses on screen */
  
  cout << "---------------------------------------------------------------" 
       << endl;
  cout << "Integration   Flux          Phase     Filename" 
       << endl;
  cout << "  Number" << endl;
  cout << "---------------------------------------------------------------" 
       << endl;
  
  for(unsigned i = 0; i < giants.size(); i++)
    cout << setw(11) << giants[i].intg << setw(10) 
	 << giants[i].flx << setw(11) << giants[i].phs 
	 << "      " << giants[i].file << endl;

  cout << endl;
  
  if ( verbose )
    cerr << "psrspa::display_giants finished" << endl;
  return;
}

void psrspa::initialise_histograms ()
{
  //TODO clean this function up, moe the phase_range initialisations somewhere else?
  if ( verbose )
    cerr << "psrspa::initialise_histograms entered" << endl;
  // determine the count of request phase_bins
  unsigned low_end, high_end;
  // if the phase range was not chosen by the user, default to full range
  if ( phase_range.size () == 0 )
  {
    phase_range.push_back ( 0.0 );
    phase_range.push_back ( 1.0 );

    hist_count = nbin;
    // populate the vector of bins for each histogram
    for ( unsigned ibin = 0; ibin <= nbin; ibin++ )
    {
      h_bin.push_back(ibin);
    }
  }
  else
  {
    for ( unsigned irange = 0; irange < phase_range.size (); irange++ )
    {
      if ( irange%2 == 0)
      {
	low_end = unsigned( floor ( phase_range[irange] * float(nbin) + 0.5) );
      }
      else
      {
	high_end = unsigned( floor ( phase_range[irange] * float(nbin) + 0.5 ) );
	if ( low_end > high_end )
	{
	  cerr << low_end << " (" << phase_range[irange-1] << ") " << high_end << " (" << phase_range[irange] << ") "<< endl;
	  throw Error ( InvalidParam, "psrspa::initialise_histograms", "wrong range provided" );
	}

	hist_count += high_end - low_end + 1;

	// populate the vector of bins for each histogram
	for ( unsigned ibin = low_end; ibin <= high_end ; ibin++ )
	{
	  h_bin.push_back(ibin);
	}
      }
    }
  }

  if ( create_polar_degree )
  {
    if ( verbose ) 
      cerr << "psrspa::initialise_histograms initialising " << hist_count << " polar degree histograms" << endl;
    for ( unsigned ihist = 0; ihist < hist_count ; ihist++ )
    {
      h_polar_degree_vec.push_back ( gsl_histogram_alloc ( polar_degree_bins ) );
      gsl_histogram_set_ranges_uniform ( h_polar_degree_vec[ihist], 0.0, 1.0 );
    }
  }
  if ( create_polar_angle )
  {
    if ( verbose ) 
      cerr << "psrspa::initialise_histograms initialising " << hist_count << " polar angle histograms" << endl;
    for ( unsigned ihist = 0; ihist < hist_count ; ihist++ )
    {
      h_polar_angle_vec.push_back ( gsl_histogram_alloc ( polar_angle_bins ) );
      gsl_histogram_set_ranges_uniform ( h_polar_angle_vec[ihist], -2.0 * M_PI, 2.0 * M_PI );
    }
  }
  if ( create_flux )
  {
    if ( verbose ) 
      cerr << "psrspa::initialise_histograms initialising " << hist_count << " phase resolved flux histograms" << endl;
    for ( unsigned ihist = 0; ihist < hist_count ; ihist++ )
    {
      h_flux_pr_vec.push_back ( gsl_histogram_alloc ( flux_pr_bins ) );
      if ( log )
	gsl_histogram_set_ranges_uniform ( h_flux_pr_vec[ihist], 0.0, 5.0 );
      else 
	gsl_histogram_set_ranges_uniform ( h_flux_pr_vec[ihist], 0.0, 100000.0 );
    }
  }
  not_initialised = false;
  if ( verbose )
    cerr << "psrspa::initialise_histograms finished" << endl;
} // initialise_histograms

void psrspa::create_histograms ( Reference::To<Archive> archive )
{
  if ( verbose )
    cerr << "psrspa::create_histograms entered" << endl;
  // ensure Stokes parameters if creating polarisation histograms
  if ( create_polar_degree || create_polar_angle )
  {
    archive->convert_state ( Signal::Stokes );
    if ( verbose )
      cerr << "psrspa::create_histograms converted state of the archive to Stokes" << endl;
  }

  // auxillary vectors
  //vector< Estimate<float > > aux_vec_f;
  vector< Estimate<double > > aux_vec_d;

  // Full Stokes profile
  Reference::To<PolnProfile> profile;
  // Polarized flux
  Reference::To<Profile> P;
  P = new Profile;
  // Total flux
  float *T_amps = new float [ nbin ];
  float *P_amps = new float [ nbin ];

  unsigned bin_min, bin_max;

  if ( verbose && max_bscrunch > 1 )
    cerr << "psrspa::create_histograms entering the bscrunch loop for the " << archive->get_filename () << " archive" << endl;
  // TODO Uh, this should be fixed up. The first idea was to enable the phase resolved histograms to be bscrunch-aware as well, but I think it doesn't make much sense so I decided to keep only the max flux bscrunch aware. This can be done much more neatly probably in such a case
  for ( current_bscrunch = 1 ; current_bscrunch <= max_bscrunch ; current_bscrunch *= 2 )
  {
    // in each passage, we bscrunch by a factor of 2
    if ( current_bscrunch > 1 )
    {
      if ( verbose )
	cerr << "psrspa::create_histograms bscrunching the archive " << archive->get_filename () << " by a factor of 2" << endl;
      archive->bscrunch ( 2 );
    }
    if ( verbose )
      cerr << "psrspa::create_histograms entering the loop through subints of the " << archive->get_filename () << " archive" << endl;
    // loop through subints
    for ( unsigned isub = 0; isub < archive->get_nsubint (); isub++ )
    {
      if ( verbose )
	cerr << "psrspa::create_histograms creating necessary profiles for subint " << isub << " of " << archive->get_filename () << endl;
      if ( create_polar_angle || create_polar_degree && current_bscrunch == 1 )
      {
	profile = archive->get_Integration(isub)->new_PolnProfile(0);
	if ( verbose )
	  cerr << "psrspa::create_histograms retrieved PolnProfile for subint " << isub << " of " << archive->get_filename () << endl;
      }
      if ( create_polar_angle && current_bscrunch == 1 )
      {
	profile->get_orientation ( aux_vec_d, 0 );
	if ( verbose )
	  cerr << "psrspa::create_histograms retrieved polarisation angle for subint " << isub << " of " << archive->get_filename () << endl;
      }
      if ( create_polar_degree || create_flux || find_max_amp_in_range )
      {
	stats.set_profile ( archive->get_Profile ( isub, 0, 0 ) );
	b_sigma = sqrt ( stats.get_baseline_variance ().get_value () );
	T_amps = archive->get_Profile ( isub, 0, 0 )->get_amps (); 
	if ( verbose )
	  cerr << "psrspa::create_histograms retrieved total flux amps for subint " << isub << " of " << archive->get_filename () << endl;
	if ( create_polar_degree && current_bscrunch == 1 )
	{
	  profile->get_polarized ( P );
	  if ( verbose )
	    cerr << "psrspa::create_histograms retrieved polarized flux profile for subint "  << isub << " of " << archive->get_filename () << endl;
	  P_amps = P->get_amps ();
	  if ( verbose )
	    cerr << "psrspa::create_histograms retrieved polarized flux amps for subint " << isub << " of " << archive->get_filename () << endl;
	}
      }

      if ( verbose )
	cerr << "psrspa::create_histograms looping through the provided phase ranges for subint " << isub << " of " << archive->get_filename () << endl;
      unsigned curr_hist = 0;
      // loop through phase ranges
      for ( unsigned irange = 0; irange < phase_range.size () ; irange++ )
      {
	if ( irange%2 == 0)
	{
	  bin_min = unsigned( floor ( phase_range[irange] * float(nbin / current_bscrunch ) + 0.5 ) );
	  if ( verbose ) 
	    cerr << "psrspa::create_histograms set minimal bin to " << bin_min << endl;
	}
	else
	{
	  bin_max = unsigned( floor ( phase_range[irange] * float(nbin / current_bscrunch ) + 0.5 ) );
	  if ( bin_max == nbin )
	    bin_max = nbin - 1 ;
	  if ( verbose ) 
	    cerr << "psrspa::create_histograms set maximum bin to " << bin_max << endl;
	  // loop through bins in the given phase range
	  for ( unsigned ibin = bin_min; ibin <= bin_max; ibin++ )
	  {
	    if ( create_polar_angle && current_bscrunch == 1 )
	    {
	      int result = gsl_histogram_increment ( h_polar_angle_vec[curr_hist], aux_vec_d[ibin].get_value () / 180.0 * M_PI );
	      if ( result == GSL_EDOM )
		warn << "WARNING psrspa::create_histograms polarisation angle the histogram range for the bin " << ibin << " in the subint " << isub << " of archive " << archive->get_filename () << endl;
	    }
	    if ( create_polar_degree && current_bscrunch == 1 )
	    {
	      // if P_amps[ibin] or T_amps[ibin] < 3 sigma, set polar degree to zero
	      int result = gsl_histogram_increment ( h_polar_degree_vec[curr_hist], ( fabs ( P_amps[ibin] ) < 3.0 * b_sigma || fabs ( T_amps[ibin] ) < 3.0 * b_sigma ) ? 0.0 : P_amps[ibin] / T_amps[ibin] );
	      if ( result == GSL_EDOM )
		warn << "WARNING psrspa::create_histograms polarisation degree outside the histogram range for the bin " << ibin << " in the subint " << isub << " of archive " << archive->get_filename () << endl;
	    }
	    if ( create_flux && current_bscrunch == 1 )
	    {
	      int result;
	      if ( log && T_amps[curr_hist] > 0.0 )
	      {
		result = gsl_histogram_increment ( h_flux_pr_vec[curr_hist], logf ( T_amps[ibin] ) );
	      }
	      else
	      {
		result = gsl_histogram_increment ( h_flux_pr_vec[curr_hist], T_amps[ibin] );
	      }
	      if ( result == GSL_EDOM )
		warn << "WARNING psrspa::create_histograms phase resolved flux outside the histogram range for the bin " << ibin << " in the subint " << isub << " of archive " << archive->get_filename () << " flux = " << T_amps[ibin] << endl;
	    }
	    // increment the histogram id
	    curr_hist ++;
	  } // loop through bins in the given phase range
	  // find the maximum amplitude in given phase range.
	  if ( find_max_amp_in_range )
	  {
	    if ( verbose )
	      cerr << "psrspa::create_histograms finding the maximum amplitude in the phase range " << irange << " of the subint " << isub << " (archive " << archive->get_filename () << ")" << endl;
	    int max_bin = archive->get_Profile ( isub, 0, 0 )->find_max_bin ( (int)bin_min, (int)bin_max );
	    identifier current_identifier = { archive->get_filename (), isub, bin_min, bin_max, current_bscrunch,
	      (unsigned)max_bin, T_amps[max_bin], b_sigma }; 
	    max_amp_info.push_back ( current_identifier );
	  } // find maximal amplitude in the given phase range
	} // handle given phase range
      } // loop through phase ranges
    } // loop through subints
  } // bscrunch loop
  if ( verbose )
    cerr << "psrspa::create_histograms finished" << endl;
}// create_histograms

void psrspa::write_histograms ()
{
  FILE *out;
  stringstream ss;

  if ( verbose )
    cerr << "psrspa::write_histograms entered" << endl;
  if ( create_flux )
  {
    if ( verbose )
      cerr << "psrspa::write_histograms writing flux histograms" << endl;
    for ( unsigned ihist = 0; ihist < hist_count; ihist++ )
    {
      ss << path+"/"+prefix+"_flux_" << h_bin[ihist] << "." << ext;
      out = fopen ( ss.str().c_str(), "w" );
      if ( ! binary_output )
	gsl_histogram_fprintf ( out, h_flux_pr_vec[ihist], "%g", "%g" );
      else
	gsl_histogram_fwrite ( out, h_flux_pr_vec[ihist] );
      fclose ( out );
      ss.str( "" );
    }
  }
  if ( create_polar_degree )
  {
    if ( verbose )
      cerr << "psrspa::write_histograms writing polarisation degree histograms" << endl;
    for ( unsigned ihist = 0; ihist < hist_count; ihist++ )
    {
      ss << path+"/"+prefix+"_poldeg_" << h_bin[ihist] << "." << ext;
      out = fopen ( ss.str().c_str(), "w" );
      if ( ! binary_output )
	gsl_histogram_fprintf ( out, h_polar_degree_vec[ihist], "%g", "%g" );
      else
	gsl_histogram_fwrite ( out, h_polar_degree_vec[ihist] );
      fclose ( out );
      ss.str( "" );
    }
  }
  if ( create_polar_angle )
  {
    if ( verbose )
      cerr << "psrspa::write_histograms writing polarisation angle histograms" << endl;
    for ( unsigned ihist = 0; ihist < hist_count; ihist++ )
    {
      ss << path+"/"+prefix+"_polang_" << h_bin[ihist] << "." << ext;
      out = fopen ( ss.str().c_str(), "w" );
      if ( ! binary_output )
	gsl_histogram_fprintf ( out, h_polar_angle_vec[ihist], "%g", "%g" );
      else
	gsl_histogram_fwrite ( out, h_polar_angle_vec[ihist] );
      fclose ( out );
      ss.str( "" );
    }
  }
  if ( find_max_amp_in_range )
  {
    ss << path+"/"+prefix+"_maxamp" << "." << ext;
    ofstream out_stream;
    if ( binary_output )
    {
      out_stream.open ( ss.str().c_str(), ios::out | ios::trunc | ios::binary );
    }
    else
      out_stream.open ( ss.str().c_str(), ios::out | ios::trunc );
    unsigned b_scrunch = 1;
    for ( unsigned ientry = 0; ientry < max_amp_info.size (); ientry++ )
    {
      out_stream << max_amp_info[ientry].file << " " << max_amp_info[ientry].integration << " " << max_amp_info[ientry].bscrunch_factor;
      for ( unsigned irange = 0; irange < phase_range.size () / 2; irange++ )
      {
	out_stream << " " << max_amp_info[ientry].phase_min << " " << max_amp_info[ientry].phase_max << " " << max_amp_info[ientry].max_amp_bin << " " << max_amp_info[ientry].max_amp_in_range << " " << max_amp_info[ientry].b_sigma ;
      }
      out_stream << endl;
    }
    //TODO for some reason this close here can cause a seg fault / bus error on Max OS X running on an Intel core.
    out_stream.close ();
  }
} // write_histograms

void psrspa::choose_bscrunch ( unsigned _max_bscrunch )
{
  max_bscrunch = _max_bscrunch;
  while ( _max_bscrunch > 1 )
  {
    if ( _max_bscrunch % 2 != 0 )
      throw Error ( InvalidParam, "psrspa::choose_bscrunch", "the max_bscrunch factor should be a power of two" ) ;
    _max_bscrunch /= 2;
  }
  perform_bscrunch_loop = true;
}
