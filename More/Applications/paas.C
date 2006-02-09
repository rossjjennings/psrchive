//
// $Id:
//
// paas -- Pulsar archive analytic standard  maker
//  (Deze programma heeft niks te maken met het christelijke feest, Pasen! :)

#include <iostream>
#include <unistd.h>

#include "Reference.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

#include "dirutil.h"
#include "string_utils.h"
#include "MEAL/ScaledVonMises.h"
#include "MEAL/ScaledVonMisesDeriv.h"
#include "MEAL/Axis.h"
#include "MEAL/LevenbergMarquardt.h"
#include "MEAL/SumRule.h"
#include "cpgplot.h"
#include <map>
#include "fftm.h"

using namespace MEAL;
using namespace Pulsar;

void usage ()
{
  cout << "paas: Pulsar archive analytic standard maker\n"
       "Usage: paas [options] archive\n"
       "Input: an archive [and optionally a model]\n"
       "Output: lekkere chocolade eiren\n";

  cout << "Options:  \n" 
    "  -r filename   Read model from file        [Default: use empty model]\n"
    "  -w filename   Write model to file         \n"
    "  -c \"a b c\"    Add component (a=centre, b=concentration, d=height)\n"
    "  -f            Fit model to pulse profile  \n"
    "  -F flags      Set parameters in/out of fit, e.g. -F 011101 fits only\n"
    "                   for concentration_1, height_1, centre_2 and height_2\n"
    "                   [Default: fit all parameters]\n"
    "  -W            Fit derivative of profile instead (i.e. weight by harm. num.)\n"
    "  -t threshold  Delta_rms/rms fit convergence criterion [Default: 1e-6]\n"
    "  -d device     Plot results on specified PGPLOT device\n"
    "  -D            Plot results on PGPLOT device /xs\n"
    "  -l            Plot with lines instead of steps\n"
    "  -b factor     Sum groups of \"factor\" adjacent bins before plotting results\n"
    "  -s filename   Write analytic standard to given filename [Default: paas.std]\n";

}

class ComponentModel
{
public:
  ComponentModel() {}

  // I/O
  void load(const char *fname);
  void unload(const char *fname) const;

  // Manipulation
  void add_component(double centre, double concentration, double height,
		     const char *name);
  void remove_component(int icomp);
  ScaledVonMises *get_component(int icomp);
  int get_ncomponents() const;

  // Fitting
  void set_infit(int icomponent, int iparam, bool infit);
  void set_infit(const char *fitstring);
  void fit(const Profile *profile, bool fit_derivative=true,
	   float threshold=1e-6);

  // Evaluating
  void evaluate(float *vals, int nvals, int icomp=-1) ;

  // Plotting
  void plot(int npts, bool line_plot=false, int icomp=-1) ;
  void plot_difference(const Profile *profile, bool line_plot=false);

protected:
  vector<ScaledVonMises> components;
  vector<string> component_names;
  map<int, string> comments; // comments, indexed by line number

  void clear();
};


void
ComponentModel::load(const char *fname)
{
  clear();
  FILE *f = fopen(fname, "r");
  if (!f)
    throw Error(FileNotFound, "ComponentModel::load");
  
  char line[1024];
  int iline=0;
  double centre, concentration, height;
  int name_start, len;
  while (fgets(line, 1023, f)!=NULL)
  {
    if (line[0]=='#') // # .. comment
      comments[iline] = line;
    else
    {
      int ic;
      bool allwhite=true;
      len = strlen(line);
      for (ic=0; ic < len && allwhite; ic++)
	if (!isspace(line[ic]))
	  allwhite = false;
      if (allwhite) // empty line
	comments[iline] = line;
      else
      {
	if (line[len-1]=='\n')
	  line[len-1]='\0';
	if (sscanf(line, "%lf %lf %lf %n", &centre, &concentration, &height,
		   &name_start)!=3)
	  throw Error(InvalidState, "ComponentModel::load", string("Could not parse file") );
	components.push_back(ScaledVonMises());
	components[components.size()-1].set_centre((centre-0.5)*2.0*M_PI);
	components[components.size()-1].set_concentration(concentration);
	components[components.size()-1].set_height(height);
	component_names.push_back(line+name_start);
      }
    }
    iline++;
  }
  fclose(f);
}

void
ComponentModel::unload(const char *fname) const
{
  FILE *f = fopen(fname, "w");
  if (!f)
    throw Error(FileNotFound, "ComponentModel::unload");
  
  int iline, icomp=0;
  bool done=false;
  map<int, string>::const_iterator comment_it;
  for (iline=0; !done; iline++)
  {
    comment_it = comments.find(iline);
    if (comment_it!=comments.end())
      fputs(comment_it->second.c_str(), f);
    else if (icomp < components.size())
    {
      fprintf(f, "%12lg %12lg %12lg %s\n", 
	      components[icomp].get_centre().val/M_PI*0.5 + 0.5,
	      components[icomp].get_concentration().val,
	      components[icomp].get_height().val,
	      component_names[icomp].c_str());
      icomp++;
    }
    else
      done = true;
  }

  fclose(f);
}

void
ComponentModel::add_component(double centre, double concentration, 
			      double height, const char *name)
{
  components.push_back(ScaledVonMises());
  components[components.size()-1].set_centre((centre-0.5)*M_PI*2.0);
  components[components.size()-1].set_concentration(concentration);
  components[components.size()-1].set_height(height);
  component_names.push_back(name);
}

void
ComponentModel::remove_component(int icomp)
{
  if (icomp < 0 || icomp >= components.size())
    throw Error(InvalidParam, "ComponentModel::remove_component",
		"Component index out of range");
  components.erase(components.begin()+icomp);
}

ScaledVonMises *
ComponentModel::get_component(int icomp)
{
  if (icomp < 0 || icomp >= components.size())
    throw Error(InvalidParam, "ComponentModel::get_component",
		"Component index out of range");
  return &components[icomp];
}

int 
ComponentModel::get_ncomponents() const
{
  return components.size();
}

void
ComponentModel::set_infit(int icomponent, int iparam, bool infit)
{
  if (icomponent < 0 || icomponent > components.size())
    throw Error(InvalidParam, "ComponentModel::get_component",
		"Component index out of range");
  if (iparam < 0 || iparam > 2)
    throw Error(InvalidParam, "ComponentModel::set_infit",
		"Parameter index out of range");

  components[icomponent].set_infit(iparam, infit);
}

void 
ComponentModel::set_infit(const char *fitstring)
{
  int iparam, icomp, ic=0, nc=strlen(fitstring);
  for (icomp=0; icomp < components.size(); icomp++)
    for (iparam=0; iparam < 3; iparam++)
    {
      if (ic==nc)
	return;
      components[icomp].set_infit(iparam, 
				       fitstring[ic]=='1'
				       ||fitstring[ic]=='t'
				       ||fitstring[ic]=='T'
				       ||fitstring[ic]=='y'
				       ||fitstring[ic]=='Y');
      printf("Set comp[%d].infit %d\n", icomp,
	     (int)(fitstring[ic]=='1'
				       ||fitstring[ic]=='t'
				       ||fitstring[ic]=='T'
				       ||fitstring[ic]=='y'
		   ||fitstring[ic]=='Y'));
      ic++;
    }
}

void 
ComponentModel::fit(const Profile *profile, bool fit_derivative,
		    float threshold)
{
  SumRule<Univariate<Scalar> > m; 
  int i, nbin=profile->get_nbin(); 
  vector<float> data(nbin);
  int icomp;

  // Construct the model and the array to fit, depending on whether
  // we work on the actual profile or its derivative
  vector<ScaledVonMisesDeriv> derivative_components(components.size());
  if (fit_derivative)
  {
    // setup model using derivative components
    for (icomp=0; icomp < components.size(); icomp++)
    {
      derivative_components[icomp].set_centre(components[icomp].get_centre());
      derivative_components[icomp].set_concentration
	(components[icomp].get_concentration());
      derivative_components[icomp].set_height(components[icomp].get_height());
      m += &derivative_components[icomp];
    }
    // setup data array as derivative of profile, using fft
//     vector<float> data_in(nbin); //... testing
//     float x;
//      for (i=0; i < nbin; i++)
//      {
//        x = (i/((double)nbin)-0.5) * 2.0 * M_PI;
//        data_in[i] = 0.018*exp(18.0*(cos(x)-1.0));
//      }
     //       data_in[i] = sin(i/((double)nbin) * 2.0 * M_PI * 2.0);
    vector<complex<float> > spec(nbin/2+2);
    /// fft::frc1d(nbin, (float*)&spec[0], &data_in[0]); // FFT
      fft::frc1d(nbin, (float*)&spec[0], profile->get_amps()); // FFT
    int ic, nc=nbin/2+1;
    float scale = 1.0/nbin;
    for (ic=0; ic < nc; ic++)
      spec[ic] *= complex<float>(0.0f, ic) // scale by i*frequency
	* scale; // correction for DFT normalisation
    fft::bcr1d(nbin, &data[0], (float*)&spec[0]); // IFFT
    //     for (i=0; i < nbin; i++)
    //    printf("%f %f\n", data[i], data_in[i]);
    //  exit(1);
  }
  else // normal profile
  {
    //Construct the summed model
    for (icomp=0; icomp < components.size(); icomp++)
      m += &components[icomp];
    // copy data
    for (i=0; i < nbin; i++)
      data[i] = profile->get_amps()[i];
  }
  
  // make data arrays for fit
  MEAL::Scalar* scalar = dynamic_cast<MEAL::Scalar*>(&m);
  MEAL::Axis<double> argument; 
  scalar->set_argument (0, &argument);
  argument.signal.connect (&m, &Univariate<Scalar>::set_abscissa);
  vector<Axis<double>::Value> xval;  
  vector< Estimate<double> > yval;
  for (i=0; i < nbin; i++)
  {
    xval.push_back(argument.get_Value((((i+0.5)/((double)nbin)-0.5)*M_PI*2.0)));
    yval.push_back(Estimate<double>(data[i], 1.0));
  }
  
  // fit
  if (!scalar) {
    throw Error(InvalidState, "ComponentModel::fit","Model is not Scalar");
  }
  MEAL::LevenbergMarquardt<double> fit;
  fit.verbose = MEAL::Function::verbose;
    
  float chisq = fit.init (xval, yval, *scalar);
  fit.singular_threshold = 1e-15; // dodge covariance problems
  unsigned iter = 1;
  unsigned not_improving = 0;
  while (not_improving < 25) { 
    //            if (iter==5) break;
    cerr << "iteration " << iter;
    float nchisq = fit.iter (xval, yval, *scalar);
    cerr << "     RMS = " << nchisq << endl;
    
    if (nchisq < chisq) {
      float diffchisq = chisq - nchisq;
      chisq = nchisq;
      not_improving = 0;
      if (diffchisq/chisq < threshold && diffchisq > 0) {
	cerr << "converged, delta RMS = " << diffchisq << endl;
	break;
      }
    }
    else
      not_improving ++;
    
    iter ++;
  }
  
  //    fit.result (*scalar);


  if (fit_derivative) // copy best-fit parameters back
  {
    for (icomp=0; icomp < components.size(); icomp++)
    {
      components[icomp].set_centre(derivative_components[icomp].get_centre());
      components[icomp].set_concentration
	(derivative_components[icomp].get_concentration());
      components[icomp].set_height(derivative_components[icomp].get_height());
    }
  }

  printf("Results of fit:\n");
  for (icomp=0; icomp < components.size(); icomp++)
    printf("Component %2d: %12lg %12lg %12lg %s\n",  icomp+1,
	   components[icomp].get_centre().val/M_PI*0.5 + 0.5,
	   components[icomp].get_concentration().val,
	   components[icomp].get_height().val,
	   component_names[icomp].c_str());


  // debugging
  //  Axis<double> argument;  
  //  m.set_argument(0,&argument);
  // argument.signal.connect (&m, &Univariate<Scalar>::set_abscissa);
#if 0
  for (i=0; i < nbin; i++) 
  { 
    argument.set_value((i/((double)nbin)-0.5)*M_PI*2.0);
    printf("%lf %lf %f\n", (i/((double)nbin)-0.5)*M_PI*2.0, 
	   m.evaluate(), data[i]);
  }
#endif
}

void 
ComponentModel::evaluate(float *vals, int nvals, int icomp_selected) 
{
  //Construct the summed model
  SumRule<Univariate<Scalar> > m; 
  if (icomp_selected >= 0)
  {
    if (icomp_selected >= components.size())
      throw Error(InvalidParam, "ComponentModel::plot",
		  "Component index out of range");
    m += &components[icomp_selected];
  }
  else
  {
    int icomp;
    for (icomp=0; icomp < components.size(); icomp++)
      m += &components[icomp];
  }

  // evaluate
  Axis<double> argument;  
  m.set_argument(0,&argument);
  argument.signal.connect (&m, &Univariate<Scalar>::set_abscissa);
  int i;
  for (i=0; i < nvals; i++)
  { 
    argument.set_value(((i+0.5)/((double)nvals)-0.5)*M_PI*2.0);
    vals[i] = m.evaluate();
  }
}


void
ComponentModel::plot(int npts, bool line_plot, int icomp_selected) 
{
  // evaluate
  vector<float> xvals(npts);
  vector<float> yvals(npts);

  evaluate(&yvals[0], npts, icomp_selected);

  int i;
  for (i=0; i < npts; i++)
    xvals[i] = i/((double)npts);

  //plot
  if (line_plot)
    cpgline(npts, &xvals[0], &yvals[0]);
  else
    cpgbin(npts, &xvals[0], &yvals[0], 0);
}

void 
ComponentModel::plot_difference(const Profile *profile, bool line_plot) 
{

  // evaluate
  int i, npts = profile->get_nbin();
  vector<float> xvals(npts);
  vector<float> yvals(npts);
  evaluate(&yvals[0], npts);

  for (i=0; i < npts; i++)
  { 
    xvals[i] = i/((double)npts);
    yvals[i] =  profile->get_amps()[i]- yvals[i];
  }

  //plot
  if (line_plot)
    cpgline(npts, &xvals[0], &yvals[0]);
  else
    cpgbin(npts, &xvals[0], &yvals[0], 0);
}


void 
ComponentModel::clear()
{
  components.clear();
  component_names.clear();
  comments.clear();
}



int main (int argc, char** argv) 
{
  const char* args = "hb:r:w:c:fF:t:d:Dl:Ws:";
  string model_filename_in, model_filename_out;
  bool fit=false;
  vector<string> new_components;
  string fit_flags;
  char c;
  int bscrunch = 1;
  float threshold=1e-6;
  char pgdev[1024];
  pgdev[0]='\0';
  char std_filename[1024];
  strcpy(std_filename, "paas.std");
  bool fit_deriv=false;
  bool line_plot=false;

  while ((c = getopt(argc, argv, args)) != -1)
    switch (c) {

    case 'h':
      usage ();
      return 0;

    case 'r':
      model_filename_in = optarg;
      break;
      
    case 'w':
      model_filename_out = optarg;
      break;
      
    case 'f':
      fit = true;
      break;

    case 'W':
      fit_deriv = true;
      break;

    case 'c':
      new_components.push_back(optarg);
      break;

    case 'F':
      fit_flags = optarg;
      break;

    case 'b':
      bscrunch = atoi (optarg);
      break;

    case 't':
      threshold = atof(optarg);
      break;

    case 'd':
      strcpy(pgdev, optarg);
      break;

    case 'D':
      strcpy(pgdev, "/xs");
      break;
      
    case 'l':
      line_plot = true;
      break;

    case 's':
      strcpy(std_filename, optarg);
      break;

    default:
      cerr << "invalid param '" << c << "'" << endl;
    }

  try
  {
    Reference::To<Pulsar::Archive> archive
      = Pulsar::Archive::load (argv[optind]);

    // preprocess
    archive->fscrunch();
    archive->tscrunch();
    archive->pscrunch();
    archive->centre();
    archive->remove_baseline();


    // make empty model
    ComponentModel model;

    // load from file if specified
    if (!model_filename_in.empty())
      model.load(model_filename_in.c_str());

    // add any new components specified
    int ic, nc=new_components.size();
    double centre, concentration, height;
    int name_start;
    for (ic=0; ic < nc; ic++)
    {
      if (sscanf(new_components[ic].c_str(), 
		 "%lf %lf %lf %n", &centre, &concentration, &height,
		 &name_start)!=3)
	throw Error(InvalidState, "main()", string("Could not parse component definition") );
      model.add_component(centre, concentration, height, 
			  new_components[ic].c_str()+name_start);
    }


    // fit if specified
    if (fit)
    {
      // set fit flags
      model.set_infit(fit_flags.c_str());
      // fit
      model.fit(archive->get_Integration(0)->get_Profile(0,0),
		fit_deriv, threshold);
    }
    // write out if specified
    if (!model_filename_out.empty())
      model.unload(model_filename_out.c_str());

    // plot
    if (pgdev[0]!='\0')
    {
      Reference::To<Pulsar::Archive> scrunched;
      scrunched = archive->clone();
      if (bscrunch > 1)
	scrunched->bscrunch(bscrunch);
      
      cpgopen(pgdev);
      cpgsvp(0.1, 0.9, 0.1, 0.9);
      Profile *prof = scrunched->get_Integration(0)->get_Profile(0,0);
      float ymin = prof->min();
      float ymax = prof->max();
      float extra = 0.05*(ymax-ymin);
      ymin -= extra;
      ymax += extra;
      cpgswin(0.0, 1.0, ymin, ymax);
      cpgbox("bcnst", 0, 0, "bcnst", 0, 0);
      cpglab("Pulse phase", "Intensity", "");
      int i, npts=prof->get_nbin();
      cpgsci(14);
      for (ic=0; ic < model.get_ncomponents(); ic++)
	model.plot(npts, true, ic);
      vector<float> xvals(npts);
      cpgsci(4);
      for (i=0; i < npts; i++)
	xvals[i] = i/((double)npts);
      if (line_plot)
	cpgline(npts, &xvals[0], prof->get_amps());
      else
	cpgbin(npts, &xvals[0], prof->get_amps(), 0);
      cpgsci(2);
      model.plot_difference(prof, line_plot);
      cpgsci(1);
      model.plot(npts, true);
      cpgend();
    }

    // write out standard
    model.evaluate(archive->get_Integration(0)->get_Profile(0,0)->get_amps(),
		   archive->get_nbin());
    archive->unload(std_filename);
  } 
  catch (Error& error) {
    cerr << error << endl;
  }
  catch (string& error) {
    cerr << error << endl;
  }
  
  return 0;
}


