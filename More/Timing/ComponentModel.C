/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ComponentModel.h"
#include "Pulsar/Profile.h"
#include "Pulsar/PhaseWeight.h"

#include "MEAL/ScaledVonMises.h"
#include "MEAL/ScaledVonMisesDeriv.h"
#include "MEAL/ScalarParameter.h"
#include "MEAL/ScalarConstant.h"

#include "MEAL/ChainRule.h"
#include "MEAL/SumRule.h"

#include "MEAL/Axis.h"
#include "MEAL/LevenbergMarquardt.h"

#include "FTransform.h"

#include <string.h>

using namespace MEAL;
using namespace std;

// #define _DEBUG

void Pulsar::ComponentModel::init ()
{
  fit_derivative = false;
  threshold = 1e-6;
}

Pulsar::ComponentModel::ComponentModel ()
{
  init ();
}

Pulsar::ComponentModel::ComponentModel (const std::string& filename)
{
  init ();
  load (filename.c_str());
}

//! Return the shift estimate
Estimate<double> Pulsar::ComponentModel::get_shift () const try
{
  if (verbose)
    cerr << "Pulsar::ComponentModel::get_shift" << endl;

  phase = new ScalarParameter (0.0);
  phase->set_value_name ("phase");

  const_cast<ComponentModel*>(this)->fit (observation);

  if (verbose)
    cerr << "Pulsar::ComponentModel::get_shift phase="
	 << phase->get_value() << " rad" << endl;

  return phase->get_Estimate(0) / (2*M_PI);
}
catch (Error& error)
{
  throw error += "Pulsar::ComponentModel::get_shift";
}

void Pulsar::ComponentModel::load (const char *fname)
{
  clear();

  FILE *f = fopen(fname, "r");
  if (!f)
    throw Error (FailedSys, "Pulsar::ComponentModel::load",
		 "fopen (%s)", fname);
  
  char line[1024];
  unsigned iline=0;

  while (fgets(line, 1023, f)!=NULL)
  {
    if (line[0]=='#') // # .. comment
      comments[iline] = line;
    else
    {
      bool allwhite=true;
      unsigned len = strlen(line);
      for (unsigned ic=0; ic < len && allwhite; ic++)
	if (!isspace(line[ic]))
	  allwhite = false;
      if (allwhite) // empty line
	comments[iline] = line;
      else
      {
	if (line[len-1]=='\n')
	  line[len-1]='\0';

	double centre, concentration, height;
	unsigned name_start;

	int scanned = sscanf (line, "%lf %lf %lf %n", 
			      &centre, &concentration, &height, &name_start);

	if (scanned !=3)
	  throw Error (InvalidState, "Pulsar::ComponentModel::load", 
		       "Could not parse file");

	components.push_back( new ScaledVonMises );
	components[components.size()-1]->set_centre(centre * 2*M_PI);
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
	      components[icomp]->get_centre().val / (2*M_PI),
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

void Pulsar::ComponentModel::add_component (double centre,
					    double concentration, 
					    double height,
					    const char *name)
{
  components.push_back( new ScaledVonMises );
  components[components.size()-1]->set_centre(centre * 2*M_PI);
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

ScaledVonMises* Pulsar::ComponentModel::get_component (unsigned icomp)
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


void Pulsar::ComponentModel::align (const Profile *profile)
{
  Profile modelprof(*profile);

  evaluate(modelprof.get_amps(), modelprof.get_nbin());

  Estimate<double> shift = profile->shift(modelprof);

  if (verbose)
    cerr << "Pulsar::ComponentModel::align shift=" << shift << endl;

  for (unsigned icomp=0; icomp < components.size(); icomp++)
  {
    Estimate<double> centre = components[icomp]->get_centre() + shift * 2*M_PI;
    if (centre.val >= M_PI)
      centre -= 2*M_PI;
    if (centre.val < -M_PI)
      centre += 2*M_PI;
    components[icomp]->set_centre(centre);
  }
}


void Pulsar::ComponentModel::set_infit (unsigned icomponent,
					unsigned iparam,
					bool infit)
{
  if (icomponent >= components.size())
    throw Error(InvalidParam, "Pulsar::ComponentModel::get_component",
		"icomponent=%u >= ncomponent=%u",
		icomponent, components.size());

  if (iparam > 2)
    throw Error(InvalidParam, "Pulsar::ComponentModel::set_infit",
		"Parameter index out of range");

  components[icomponent]->set_infit (iparam, infit);
}

bool get_boolean (char c)
{
  return c=='1' || c=='t' || c=='T' || c=='y' || c=='Y';
}

void Pulsar::ComponentModel::set_infit (const char *fitstring)
{
  int ic=0, nc=strlen(fitstring);

  for (unsigned icomp=0; icomp < components.size(); icomp++)
    for (unsigned iparam=0; iparam < 3; iparam++)
    {
      if (ic==nc)
	return;

      bool fit = get_boolean (fitstring[ic]);

      components[icomp]->set_infit(iparam, fit);

      if (verbose)
	cerr << "Pulsar::ComponentModel::set_infit icomp=" << icomp
	     << " iparam=" << iparam << " fit=" << fit << endl;
      ic++;
    }
}

void Pulsar::ComponentModel::fit (const Profile *profile)
{
  SumRule< Univariate<Scalar> > sum; 

  unsigned nbin = profile->get_nbin(); 
  vector<float> data (nbin);

  float mean = 0.0;
  float variance = 1.0;

  // Construct the model and the array to fit, depending on whether
  // we work on the actual profile or its derivative

  vector< Reference::To<ScaledVonMisesDeriv> > derivative;

  if (fit_derivative)
  {
    derivative.resize (components.size());

    // setup model using derivative components
    for (unsigned icomp=0; icomp < components.size(); icomp++)
    {
      derivative[icomp] = new ScaledVonMisesDeriv;

      derivative[icomp]->set_centre(components[icomp]->get_centre());
      derivative[icomp]->set_concentration
	(components[icomp]->get_concentration());
      derivative[icomp]->set_height(components[icomp]->get_height());
      sum += derivative[icomp];
    }

    vector<complex<float> > spec(nbin/2+2);

    // FFT
    FTransform::frc1d(nbin, (float*)&spec[0], profile->get_amps());

    unsigned ic, nc=nbin/2+1;
    float scale = 1.0/nbin;
    for (ic=0; ic < nc; ic++)
      spec[ic] *= complex<float>(0.0f, ic) // scale by i*frequency
	* scale; // correction for DFT normalisation

    // IFFT
    FTransform::bcr1d (nbin, &data[0], (float*)&spec[0]);
  }
  else // normal profile
  {
    // Construct the summed model
    for (unsigned icomp=0; icomp < components.size(); icomp++)
      sum += components[icomp];

    Reference::To<PhaseWeight> baseline = profile->baseline();
    variance = baseline->get_variance().get_value();
    mean = baseline->get_mean().get_value();

    if (verbose)
      cerr << "Pulsar::ComponentModel::fit mean=" << mean
	   << " variance=" << variance << endl;

    // copy data
    for (unsigned i=0; i < nbin; i++)
      data[i] = profile->get_amps()[i] - mean;
  }

  Reference::To< Univariate<Scalar> > scalar = &sum;

  if (phase)
  {
    if (verbose)
      cerr << "Pulsar::ComponentModel::fit single phase" << endl;

    ChainRule<Univariate<Scalar> >* chain = new ChainRule<Univariate<Scalar> >;
    scalar = chain;

    chain->set_model (&sum);

    for (unsigned ic=0; ic < components.size(); ic++)
    {
      if (sum.get_param_name(ic*3) != "centre")
	throw Error (InvalidState, "Pulsar::ComponentModel::fit",
		     "iparam=%u name='%s'", ic*3,
		     sum.get_param_name(ic*3).c_str());

      SumRule<Scalar>* psum = new SumRule<Scalar>;
      psum->add_model (phase);
      psum->add_model (new ScalarConstant (sum.get_param(ic*3)));

      chain->set_constraint (ic*3, psum);
    }
  }

#ifdef _DEBUG

  cerr << "Pulsar::ComponentModel::fit nparam=" << scalar->get_nparam() <<endl;
  for (unsigned i=0; i<scalar->get_nparam(); i++)
    cerr << i 
	 << " " << scalar->get_param_name(i)
	 << " " << scalar->get_param(i)
	 << " " << scalar->get_infit(i)
	 << endl;
#endif

  Axis<double> argument; 
  scalar->set_argument (0, &argument);

  vector< Axis<double>::Value > xval;  
  vector< Estimate<double> > yval;

  unsigned nfree = 0;

  for (unsigned i=0; i < nbin; i++)
  {
    double radians = (i+0.5)/nbin * 2*M_PI;

    xval.push_back( argument.get_Value(radians) );
    yval.push_back( Estimate<double>(data[i], variance) );

    nfree ++;
  }
  
  for (unsigned i=0; i<scalar->get_nparam(); i++)
    if (scalar->get_infit(i))
    {
      if (nfree == 0)
	throw Error (InvalidState, "Pulsar::ComponentModel::fit",
		     "more free parameters than data");
      nfree --;
    }

  // fit

  LevenbergMarquardt<double> fit;
  fit.verbose = Function::verbose;
    
  chisq = fit.init (xval, yval, *scalar);

  // fit.singular_threshold = 1e-15; // dodge covariance problems
  unsigned iter = 1;
  unsigned not_improving = 0;

  string result = "RMS";
  if (variance)
    result = "chisq";

  while (not_improving < 25)
  { 
    if (verbose)
      cerr << "iteration " << iter;

    float nchisq = fit.iter (xval, yval, *scalar);

    if (verbose)
      cerr << "     " << result << " = " << nchisq << endl;
    
    if (nchisq < chisq)
    {
      float diffchisq = chisq - nchisq;
      chisq = nchisq;
      not_improving = 0;

      if (diffchisq/chisq < threshold && diffchisq > 0)
      {
	if (verbose)
	  cerr << "converged, delta " << result << " = " << diffchisq << endl;
	break;
      }
    }
    else
      not_improving ++;
    
    iter ++;
  }
  
  std::vector<std::vector<double> > covar;
  fit.result (*scalar, covar);
  
  for (unsigned iparam=0; iparam < scalar->get_nparam(); iparam++)
  {
    double variance = covar[iparam][iparam];

    if (verbose)
      cerr << "Pulsar::ComponentModel::fit"
	" variance[" << iparam << "]=" << variance << endl;

    if (!finite(variance))
      throw Error (InvalidState, 
		   "Pulsar::ComponentModel::fit",
		   "non-finite variance "
		   + scalar->get_param_name(iparam));

    if (!scalar->get_infit(iparam) && variance != 0)
      throw Error (InvalidState,
		   "Pulsar::ComponentModel::fit",
		   "non-zero unfit variance "
		   + scalar->get_param_name(iparam)
		   + " = " + tostring(variance) );

    if (variance < 0)
      throw Error (InvalidState,
		   "Pulsar::ComponentModel::fit",
		   "invalid variance " + scalar->get_param_name(iparam)
		   + " = " + tostring(variance) );

    // cerr << iparam << ".var=" << variance << endl;

    scalar->set_variance (iparam, variance);
  }

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
}

void Pulsar::ComponentModel::evaluate (float *vals,
				       unsigned nvals, int icomp_selected) 
{
  //Construct the summed model
  SumRule< Univariate<Scalar> > m; 
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
  Axis<double> argument;  
  m.set_argument (0, &argument);

  for (unsigned i=0; i < nvals; i++)
  { 
    argument.set_value( (i+0.5)/nvals * 2*M_PI );
    vals[i] = m.evaluate();
  }
}

void Pulsar::ComponentModel::clear()
{
  components.clear();
  component_names.clear();
  comments.clear();
}
