/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ComponentModel.h"
#include "Pulsar/Profile.h"

#include "MEAL/ScaledVonMises.h"
#include "MEAL/ScaledVonMisesDeriv.h"
#include "MEAL/Axis.h"
#include "MEAL/LevenbergMarquardt.h"
#include "MEAL/SumRule.h"

#include "FTransform.h"

using namespace std;

Pulsar::ComponentModel::ComponentModel ()
{
  fit_derivative = false;
  threshold = 1e-6;
}

//! Return the shift estimate
Estimate<double> Pulsar::ComponentModel::get_shift () const
{
  throw Error (InvalidState, "Pulsar::ComponentModel::get_shift",
	       "not implemented");
}

void Pulsar::ComponentModel::load (const char *fname)
{
  clear();

  FILE *f = fopen(fname, "r");
  if (!f)
    throw Error (FailedSys, "Pulsar::ComponentModel::load",
		 "fopen (%s)", fname);
  
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
	  throw Error(InvalidState, "Pulsar::ComponentModel::load", string("Could not parse file") );

	components.push_back( new MEAL::ScaledVonMises );
	components[components.size()-1]->set_centre((centre-0.5)*2.0*M_PI);
	components[components.size()-1]->set_concentration(concentration);
	components[components.size()-1]->set_height(height);
	component_names.push_back(line+name_start);
      }
    }
    iline++;
  }
  fclose(f);
}

void
Pulsar::ComponentModel::unload(const char *fname) const
{
  FILE *f = fopen(fname, "w");
  if (!f)
    throw Error(FileNotFound, "Pulsar::ComponentModel::unload");
  
  unsigned iline, icomp = 0;
  bool done=false;
  map<unsigned, string>::const_iterator comment_it;
  for (iline=0; !done; iline++)
  {
    comment_it = comments.find(iline);
    if (comment_it!=comments.end())
      fputs(comment_it->second.c_str(), f);
    else if (icomp < components.size())
    {
      fprintf(f, "%12lg %12lg %12lg %s\n", 
	      components[icomp]->get_centre().val/M_PI*0.5 + 0.5,
	      components[icomp]->get_concentration().val,
	      components[icomp]->get_height().val,
	      component_names[icomp].c_str());
      icomp++;
    }
    else
      done = true;
  }

  fclose(f);
}

void
Pulsar::ComponentModel::add_component(double centre, double concentration, 
			      double height, const char *name)
{
  components.push_back( new MEAL::ScaledVonMises );
  components[components.size()-1]->set_centre((centre-0.5)*M_PI*2.0);
  components[components.size()-1]->set_concentration(concentration);
  components[components.size()-1]->set_height(height);
  component_names.push_back(name);
}

void Pulsar::ComponentModel::remove_component (unsigned icomp)
{
  if (icomp >= components.size())
    throw Error(InvalidParam, "Pulsar::ComponentModel::remove_component",
		"Pulsar::Component index out of range");
  components.erase(components.begin()+icomp);
}

MEAL::ScaledVonMises* Pulsar::ComponentModel::get_component (unsigned icomp)
{
  if (icomp >= components.size())
    throw Error(InvalidParam, "Pulsar::ComponentModel::get_component",
		"Component index out of range");
  return components[icomp];
}

unsigned Pulsar::ComponentModel::get_ncomponents() const
{
  return components.size();
}


void  Pulsar::ComponentModel::align (const Profile *profile)
{
  Profile modelprof(*profile);

  evaluate(modelprof.get_amps(), modelprof.get_nbin());

  Estimate<double> shift = profile->shift(modelprof);

  printf("Shift= %.6lf\n", shift.val);

  for (unsigned icomp=0; icomp < components.size(); icomp++)
  {
    Estimate<double> centre = components[icomp]->get_centre()+  shift*M_PI*2.0;
    if (centre.val >= M_PI)
      centre -= M_PI*2.0;
    if (centre.val < -M_PI)
      centre += M_PI*2.0;
    components[icomp]->set_centre(centre);
  }
}


void Pulsar::ComponentModel::set_infit (unsigned icomponent,
					unsigned iparam,
					bool infit)
{
  if (icomponent > components.size())
    throw Error(InvalidParam, "Pulsar::ComponentModel::get_component",
		"Component index out of range");

  if (iparam > 2)
    throw Error(InvalidParam, "Pulsar::ComponentModel::set_infit",
		"Parameter index out of range");

  components[icomponent]->set_infit (iparam, infit);
}

void Pulsar::ComponentModel::set_infit (const char *fitstring)
{
  int ic=0, nc=strlen(fitstring);

  for (unsigned icomp=0; icomp < components.size(); icomp++)
    for (unsigned iparam=0; iparam < 3; iparam++)
    {
      if (ic==nc)
	return;
      components[icomp]->set_infit(iparam, 
				       fitstring[ic]=='1'
				       ||fitstring[ic]=='t'
				       ||fitstring[ic]=='T'
				       ||fitstring[ic]=='y'
				       ||fitstring[ic]=='Y');
      printf("Set comp[%d]->infit %d\n", icomp,
	     (int)(fitstring[ic]=='1'
				       ||fitstring[ic]=='t'
				       ||fitstring[ic]=='T'
				       ||fitstring[ic]=='y'
		   ||fitstring[ic]=='Y'));
      ic++;
    }
}

void Pulsar::ComponentModel::fit (const Profile *profile)
{
  MEAL::SumRule< MEAL::Univariate<MEAL::Scalar> > m; 

  unsigned i, nbin=profile->get_nbin(); 
  vector<float> data(nbin);

  // Construct the model and the array to fit, depending on whether
  // we work on the actual profile or its derivative

  vector< Reference::To<MEAL::ScaledVonMisesDeriv> > derivative;

  if (fit_derivative)
  {
    derivative.resize (components.size());

    // setup model using derivative components
    for (unsigned icomp=0; icomp < components.size(); icomp++)
    {
      derivative[icomp] = new MEAL::ScaledVonMisesDeriv;

      derivative[icomp]->set_centre(components[icomp]->get_centre());
      derivative[icomp]->set_concentration
	(components[icomp]->get_concentration());
      derivative[icomp]->set_height(components[icomp]->get_height());
      m += derivative[icomp];
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
      FTransform::frc1d(nbin, (float*)&spec[0], profile->get_amps()); // FFT
    int ic, nc=nbin/2+1;
    float scale = 1.0/nbin;
    for (ic=0; ic < nc; ic++)
      spec[ic] *= complex<float>(0.0f, ic) // scale by i*frequency
	* scale; // correction for DFT normalisation
    FTransform::bcr1d(nbin, &data[0], (float*)&spec[0]); // IFFT
    //     for (i=0; i < nbin; i++)
    //    printf("%f %f\n", data[i], data_in[i]);
    //  exit(1);
  }
  else // normal profile
  {
    //Construct the summed model
    for (unsigned icomp=0; icomp < components.size(); icomp++)
      m += components[icomp];
    // copy data
    for (i=0; i < nbin; i++)
      data[i] = profile->get_amps()[i];
  }
  
  // make data arrays for fit
  MEAL::Scalar* scalar = dynamic_cast<MEAL::Scalar*>(&m);
  MEAL::Axis<double> argument; 
  scalar->set_argument (0, &argument);
  argument.signal.connect (&m, &MEAL::Univariate<MEAL::Scalar>::set_abscissa);
  vector< MEAL::Axis<double>::Value > xval;  
  vector< Estimate<double> > yval;
  for (i=0; i < nbin; i++)
  {
    xval.push_back(argument.get_Value((((i+0.5)/((double)nbin)-0.5)*M_PI*2.0)));
    yval.push_back(Estimate<double>(data[i], 1.0));
  }
  
  // fit
  if (!scalar) {
    throw Error(InvalidState, "Pulsar::ComponentModel::fit","Model is not Scalar");
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
    for (unsigned icomp=0; icomp < components.size(); icomp++)
    {
      components[icomp]->set_centre(derivative[icomp]->get_centre());
      components[icomp]->set_concentration
	(derivative[icomp]->get_concentration());
      components[icomp]->set_height(derivative[icomp]->get_height());
    }
  }

  printf("Results of fit:\n");
  for (unsigned icomp=0; icomp < components.size(); icomp++)
    printf("Component %2d: %12lg %12lg %12lg %s\n",  icomp+1,
	   components[icomp]->get_centre().val/M_PI*0.5 + 0.5,
	   components[icomp]->get_concentration().val,
	   components[icomp]->get_height().val,
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
Pulsar::ComponentModel::evaluate(float *vals, unsigned nvals, int icomp_selected) 
{
  //Construct the summed model
  MEAL::SumRule< MEAL::Univariate<MEAL::Scalar> > m; 
  if (icomp_selected >= 0)
  {
    if (icomp_selected >= (int)components.size())
      throw Error(InvalidParam, "Pulsar::ComponentModel::plot",
		  "Component index out of range");
    m += components[icomp_selected];
  }
  else
  {
    for (unsigned icomp=0; icomp < components.size(); icomp++)
      m += components[icomp];
  }

  // evaluate
  MEAL::Axis<double> argument;  
  m.set_argument(0,&argument);
  argument.signal.connect (&m, &MEAL::Univariate<MEAL::Scalar>::set_abscissa);

  for (unsigned i=0; i < nvals; i++)
  { 
    argument.set_value(((i+0.5)/((double)nvals)-0.5)*M_PI*2.0);
    vals[i] = m.evaluate();
  }
}

void Pulsar::ComponentModel::clear()
{
  components.clear();
  component_names.clear();
  comments.clear();
}
