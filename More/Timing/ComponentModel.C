/***************************************************************************
 *
 *   Copyright (C) 2009 - 2017 by Willem van Straten
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
#include <stdio.h>
#include <iomanip>

using namespace Pulsar;
using namespace MEAL;
using namespace std;

// #define _DEBUG

void ComponentModel::init ()
{
  gate = 1.0;
  
  fit_derivative = false;
  log_height = false;
  retain_memory = false;
  fix_widths = false;
  fit_primary_first = false;
  
  report_absolute_phases = false;
  report_widths = false;
  
  zap_height_ratio = 0.0;
  zap_concentration_ratio = 0.0;

  threshold = 1e-3;
}

ComponentModel::ComponentModel ()
{
  init ();
}

ComponentModel::ComponentModel (const std::string& filename)
{
  init ();
  load (filename.c_str());
}

MEAL::Univariate<MEAL::Scalar>* ComponentModel::get_model ()
{
  if (!model)
    build ();
  return model;
}

void ComponentModel::fix_relative_phases ()
{
  if (phase)
    return;

  if (model)
    throw Error (InvalidState, "ComponentModel::fix_relative_phases",
		 "cannot fix relative phases after model is built");

  phase = new ScalarParameter (0.0);
  phase->set_value_name ("phase");
}

void ComponentModel::set_report_absolute_phases (bool flag)
{
  if (flag)
    fix_relative_phases ();

  if (!flag && phase)
    throw Error (InvalidState, "ComponentModel::set_report_absolute_phases",
		 "cannot unfix absolute phases after model is built");

  report_absolute_phases = flag;
}

void ComponentModel::set_log_height (bool flag)
{
  if (components.size() != 0)
    throw Error (InvalidState, "ComponentModel::set_log_height",
		 "cannot change interpretation of height after components have been added");

  log_height = flag;
}

void ComponentModel::set_retain_memory (bool flag)
{
  if (model)
    throw Error (InvalidState, "ComponentModel::set_retain_memory",
		 "cannot change flag to retain memory after model is built");

  retain_memory = flag;
}

void ComponentModel::set_fix_widths (bool flag)
{
  if (model)
    throw Error (InvalidState, "ComponentModel::set_fix_widths",
		 "cannot change flag to fix widths after model is built");

  fix_widths = flag;
}

void ComponentModel::set_fit_primary_first (bool flag)
{
  fit_primary_first = flag;
}

//! Return the shift estimate
Estimate<double> ComponentModel::get_shift () const try
{
  if (verbose)
    cerr << "ComponentModel::get_shift" << endl;

  if (backup.size() == components.size())
  {
    if (verbose)
      cerr << "ComponentModel::get_shift restore backup" << endl;

    for (unsigned i=0; i<components.size(); i++)
    {
      components[i]->set_height( backup[i]->get_height() );
      components[i]->set_concentration( backup[i]->get_concentration() );
    }
  }

  try
  {
    const_cast<ComponentModel*>(this)->align (observation);

    if (fit_primary_first)
    {
#if _DEBUG
      cerr << "ComponentModel::get_shift fitting primary first" << endl;
#endif
      if (backup.size() == components.size())
      for (unsigned i=1; i<components.size(); i++)
      {
	freeze (i);
#if _DEBUG
        cerr << "after freeze fits:";
        unsigned nparam = components[i]->get_nparam ();
        for (unsigned ip=0; ip < nparam; ip++)
          cerr << " " << components[i]->get_infit(ip);
        cerr << endl;
#endif
      }

      const_cast<ComponentModel*>(this)->fit (observation);
      for (unsigned i=1; i<components.size(); i++)
      {
	unfreeze (i);
#if _DEBUG
        cerr << "after unfreeze fits:";
        unsigned nparam = components[i]->get_nparam ();
        for (unsigned ip=0; ip < nparam; ip++)
          cerr << " " << components[i]->get_infit(ip);
        cerr << endl;
#endif
      } 
    }
    
    const_cast<ComponentModel*>(this)->fit (observation);

    if (retain_memory && get_reduced_chisq() < 2.0)
      for (unsigned i=0; i<components.size(); i++)
      {
	backup[i]->set_height( components[i]->get_height() );
	backup[i]->set_concentration( components[i]->get_concentration() );
      }
  }
  catch (Error& error)
  {
    throw error += "ComponentModel::get_shift";
  }

  if (verbose)
    cerr << "ComponentModel::get_shift phase="
	 << phase->get_value() << " rad" << endl;

  return phase->get_Estimate(0) / (2*M_PI);
}
catch (Error& error)
{
  throw error += "ComponentModel::get_shift";
}

double ComponentModel::get_absolute_phase () const
{
  if (!phase)
    throw Error (InvalidState, "ComponentModel::get_absolute_phase",
		 "absolute phase not initialized");
  
  return phase->get_value().val;
}

void ComponentModel::load (const char *fname)
{
  clear();

  FILE *f = fopen(fname, "r");
  if (!f)
    throw Error (FailedSys, "ComponentModel::load",
		 "fopen (%s)", fname);
  
  char line[1024];
  unsigned iline=0;

  filename = fname;
  
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
      else if (line == string("log height\n"))
      {
	cerr << "ComponentModel::load logarithm of height" << endl;
	set_log_height (true);
      }
      else if (line == string ("fix relative phases\n"))
      {
	cerr << "ComponentModel::load fix relative phases" << endl;
	fix_relative_phases ();
      }
      else if (line == string ("fix widths\n"))
      {
	cerr << "ComponentModel::load fix widths" << endl;
	set_fix_widths (true);
      }
      else if (line == string ("fit primary first\n"))
      {
	cerr << "ComponentModel::load fit primary first" << endl;
	set_fit_primary_first (true);
      }
      else if (line == string ("retain memory\n"))
      {
	cerr << "ComponentModel::load retain memory" << endl;
	set_retain_memory (true);
      }
      else if (line == string ("report absolute phases\n"))
      {
	cerr << "ComponentModel::load report absolute phases" << endl;
	set_report_absolute_phases (true);
      }
      // don't forget to add a matching line to ComponentModel::unload
      else
      {
	if (line[len-1]=='\n')
	  line[len-1]='\0';

	double centre, concentration, height;
	unsigned name_start;

	int scanned = sscanf (line, "%lf %lf %lf %n", 
			      &centre, &concentration, &height, &name_start);

	if (scanned !=3)
	  throw Error (InvalidState, "ComponentModel::load", 
		       "Could not parse file");

	components.push_back( new ScaledVonMises (log_height) );
	components[components.size()-1]->set_centre(centre * 2*M_PI);
	if (concentration > 1.0)
	  components[components.size()-1]->set_concentration(concentration);
	else
	{
	  components[components.size()-1]->set_width(concentration * 2*M_PI);
	  double test = components[components.size()-1]->get_width() / (2*M_PI);

	  cerr << "width set=" << concentration << " get=" << test << endl;
	}
	components[components.size()-1]->set_height(height);
	component_names.push_back(line+name_start);
      }
    }
    iline++;
  }
  fclose(f);
}

void ComponentModel::unload (const char *fname) const
{
  FILE *f = fopen(fname, "w");
  if (!f)
    throw Error (FailedSys, "ComponentModel::unload",
		 "fopen (%s)", fname);

  double phase_offset = 0;
  if (report_absolute_phases)
    phase_offset = phase->get_value().val;
  
  // add lines here to match ComponentModel::load
  if (log_height)
    fprintf (f, "log height\n");

  if (report_absolute_phases)
    fprintf (f, "report absolute phases\n");
  else if (phase)
    fprintf (f, "fix relative phases\n");

  if (fix_widths)
    fprintf (f, "fix widths\n");

  if (fit_primary_first)
    fprintf (f, "fit primary first\n");

  if (retain_memory)
    fprintf (f, "retain memory\n");

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
      double kappa_or_sigma = components[icomp]->get_concentration().val;
      if (report_widths)
	kappa_or_sigma = components[icomp]->get_width () / (2*M_PI);
      
      fprintf(f, "%12lg %12lg %12lg %s\n", 
	      (components[icomp]->get_centre().val - phase_offset) / (2*M_PI),
	      kappa_or_sigma,
	      components[icomp]->get_height().val,
	      component_names[icomp].c_str());
      icomp++;
    }
    else
      done = true;
  }

  fclose(f);
}

void ComponentModel::add_component (double centre,
					    double concentration, 
					    double height,
					    const char *name)
{
  if (log_height && height <= 0)
    throw Error (InvalidParam, "ComponentModel::add_component",
		 "log_height==true and height=%f", height);
  
  components.push_back( new ScaledVonMises (log_height) );
  components[components.size()-1]->set_centre(centre * 2*M_PI);
  components[components.size()-1]->set_concentration(concentration);
  components[components.size()-1]->set_height(height);
  component_names.push_back(name);

  model = 0;
}

void ComponentModel::check (const char* method, unsigned icomp) const
{
  if (icomp >= components.size())
    throw Error (InvalidParam, "ComponentModel::" + string(method),
		 "icomponent=%u >= ncomponent=%u",
		 icomp, components.size());
}

void ComponentModel::remove_component (unsigned icomp)
{
  check ("remove_component", icomp);
  components.erase (components.begin()+icomp);
}

ScaledVonMises* ComponentModel::get_component (unsigned icomp)
{
  check ("get_component", icomp);
  return components[icomp];
}

unsigned ComponentModel::get_ncomponents() const
{
  return components.size();
}

/*! rotates the profile to match the phase of the model */
void ComponentModel::align_to_model (Profile *profile)
{
  double the_phase = 0;
  double the_scale = 0;
  get_best_alignment (profile, the_phase, the_scale);

  if (verbose)
    cerr << "ComponentModel::align_to_model"
      " phase=" << the_phase << endl;
  
  profile->rotate_phase (the_phase);
  profile->scale (1/the_scale);
}

/* returns the phase shift and scale that aligns the profile with the model */
void ComponentModel::get_best_alignment (const Profile* profile, double& the_phase, double& the_scale)
{
  Profile modelprof (profile->get_nbin());

  if (verbose)
    cerr << "ComponentModel::get_best_alignment calling evaluate" << endl;
  
  evaluate (modelprof.get_amps(), modelprof.get_nbin());

  Estimate<double> shift = profile->shift (modelprof);

  if (verbose)
    cerr << "ComponentModel::get_best_alignment shift=" << shift << endl;

  the_phase = shift.get_value();

  the_scale = profile->sum() / modelprof.sum();

  if (verbose)
    cerr << "ComponentModel::get_best_alignment scale=" << the_scale << endl;
}

void ComponentModel::align (const Profile *profile)
{
  if (verbose)
    cerr << "ComponentModel::align (model to data)" << endl;

  double the_phase = 0;
  double the_scale = 0;
  get_best_alignment (profile, the_phase, the_scale);

  for (unsigned icomp=0; icomp < components.size(); icomp++)
  {
    Estimate<double> height = components[icomp]->get_height();
    components[icomp]->set_height ( height * the_scale );
  }

  if (phase)
  {
    if (verbose)
      cerr << "ComponentModel::align increment phase by "
	   << the_phase << " turns" << endl;
    phase->set_value (phase->get_value() + the_phase * 2*M_PI);
    return;
  }

  // Dick requires this
  cout << setprecision(6) << fixed << "Shift= " << the_phase << endl;

  for (unsigned icomp=0; icomp < components.size(); icomp++)
  {
    Estimate<double> centre = components[icomp]->get_centre() + the_phase*2*M_PI;
    if (centre.val >= M_PI)
      centre -= 2*M_PI;
    if (centre.val < -M_PI)
      centre += 2*M_PI;
    components[icomp]->set_centre(centre);
  }
}

void ComponentModel::freeze (unsigned icomponent) const
{
  check ("freeze", icomponent);

  if (backup.size() != components.size())
    throw Error (InvalidState, "ComponentModel::unfreeze",
                 "should not freeze without backup");

  unsigned nparam = components[icomponent]->get_nparam();
  for (unsigned iparam=0; iparam<nparam; iparam++)
    components[icomponent]->set_infit (iparam, false);
}

void ComponentModel::unfreeze (unsigned icomponent) const
{
  check ("unfreeze", icomponent);

  if (backup.size() != components.size())
    throw Error (InvalidState, "ComponentModel::unfreeze",
		 "cannot unfreeze without backup");

  unsigned nparam = components[icomponent]->get_nparam();
  for (unsigned ip=0; ip<nparam; ip++)
    components[icomponent]->set_infit (ip, backup[icomponent]->get_infit(ip));
}

void ComponentModel::set_infit (unsigned icomponent,
					unsigned iparam,
					bool infit)
{
  check ("set_infit", icomponent);

  if (iparam > 2)
    throw Error(InvalidParam, "ComponentModel::set_infit",
		"Parameter index out of range");

  components[icomponent]->set_infit (iparam, infit);
}

bool get_boolean (char c)
{
  return c=='1' || c=='t' || c=='T' || c=='y' || c=='Y';
}

void ComponentModel::set_infit (const char *fitstring)
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
	cerr << "ComponentModel::set_infit icomp=" << icomp
	     << " iparam=" << iparam << " fit=" << fit << endl;
      ic++;
    }
}

void ComponentModel::build () const
{
  if (model)
    return;

  SumRule< Univariate<Scalar> >* sum = new SumRule< Univariate<Scalar> >; 
  model = sum;

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
      sum->add_model( derivative[icomp] );
    }
  }
  else
  {
    for (unsigned icomp=0; icomp < components.size(); icomp++)
    {
      sum->add_model( components[icomp] );
      // don't allow the widths to vary
      if (fix_widths)
	components[icomp]->set_infit(1, false);
    }
  }

  if (phase)
  {
    if (verbose)
      cerr << "ComponentModel::build single phase" << endl;

    backup.resize (components.size());

    ChainRule<Univariate<Scalar> >* chain = new ChainRule<Univariate<Scalar> >;
    chain->set_model (sum);

    model = chain;

    for (unsigned icomp=0; icomp < components.size(); icomp++)
    {
#if _DEBUG
      cerr << "BEFORE CLONE fits:";
      unsigned nparam = components[icomp]->get_nparam ();
      for (unsigned ip=0; ip < nparam; ip++)
	cerr << " " << components[icomp]->get_infit(ip);
      cerr << endl;
#endif

      backup[icomp] = components[icomp]->clone();

#if _DEBUG
      cerr << "AFTER CLONE fits:";
      nparam = components[icomp]->get_nparam ();
      for (unsigned ip=0; ip < nparam; ip++)
	cerr << " " << backup[icomp]->get_infit(ip);
      cerr << endl;
#endif

      SumRule<Scalar>* psum = new SumRule<Scalar>;
      psum->add_model (phase);
      psum->add_model (new ScalarConstant (sum->get_param(icomp*3)));

      // verify that iparam=icomp*3 corresponds to component centre
      if (chain->get_param_name(icomp*3) != "centre")
	throw Error (InvalidState, "ComponentModel::build",
		     "iparam=%u name='%s'", icomp*3,
		     sum->get_param_name(icomp*3).c_str());

      chain->set_constraint (icomp*3, psum);
    }
  }
}


void ComponentModel::fit (const Profile *profile) try
{
  build ();

  unsigned nbin = profile->get_nbin(); 
  vector<float> data (nbin);

  float mean = 0.0;
  float variance = 1.0;

  if (fit_derivative)
  {
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
    Reference::To<PhaseWeight> baseline = profile->baseline();
    variance = baseline->get_variance().get_value();
    mean = baseline->get_mean().get_value();

    if (verbose)
      cerr << "ComponentModel::fit mean=" << mean
	   << " variance=" << variance << endl;

    // copy data
    for (unsigned i=0; i < nbin; i++)
      data[i] = profile->get_amps()[i] - mean;
  }

  Axis<double> argument; 
  model->set_argument (0, &argument);

  vector< Axis<double>::Value > xval;  
  vector< Estimate<double> > yval;

  nfree = 0;

  for (unsigned i=0; i < nbin; i++)
  {
    double radians = (i+0.5) * gate / nbin * 2*M_PI;

    xval.push_back( argument.get_Value(radians) );
    yval.push_back( Estimate<double>(data[i], variance) );

    nfree ++;
  }
  
  for (unsigned i=0; i<model->get_nparam(); i++)
    if (model->get_infit(i))
    {
      if (nfree == 0)
	throw Error (InvalidState, "ComponentModel::fit",
		     "more free parameters than data");
      nfree --;
    }

  // fit

  LevenbergMarquardt<double> fit;
  fit.verbose = Function::verbose;

#if 0 // for tweaking
  fit.lamda = 1e-5;
  fit.lamda_increase_factor = 10;
  fit.lamda_decrease_factor = 0.5;
#endif
  
  chisq = fit.init (xval, yval, *model);

  // fit.singular_threshold = 1e-15; // dodge covariance problems
  unsigned iter = 1;
  unsigned not_improving = 0;

  string result = "RMS";
  if (variance)
    result = "chisq";

  unsigned max_iter = 100;

  while (not_improving < 25 && iter < max_iter)
  { 
    if (verbose)
      cerr << "iteration " << iter;

    float nchisq = fit.iter (xval, yval, *model);

    if (verbose)
    {
      cerr << "     " << result << " = " << nchisq << endl;

      for (unsigned i=0; i < components.size(); i++)
	  cerr << "\t" "height[" << i << "]=" << components[i]->get_height() << endl;
    }

    bool component_removed = false;

    if (zap_height_ratio != 0.0)
    {
      cerr << "auto clean height ratio = " << zap_height_ratio << endl;
      
      float max_height = 0.0;
      for (unsigned i=0; i < components.size(); i++)
	if (components[i]->get_height() > max_height)
	  max_height = components[i]->get_height().val;
      
      unsigned i=0; 
      while (i < components.size())
      {
	float height_ratio = components[i]->get_height().val / max_height;
	
	if (height_ratio < zap_height_ratio)
        {
	  cerr << "removing component " << i 
	       << " height=" << components[i]->get_height();
	  
	  components.erase( components.begin() + i );
	  component_removed = true;
	}
	else
	  i++;
      }
    }
    
    if (zap_concentration_ratio != 0.0)
    {
      cerr << "auto clean factor = " << zap_concentration_ratio << endl;
      
      float min_concentration = components[0]->get_concentration().val;
      for (unsigned i=1; i < components.size(); i++)
	if (components[i]->get_concentration() < min_concentration)
	  min_concentration = components[i]->get_concentration().val;
      
      unsigned i=0; 
      while (i < components.size())
      {
	float ratio = components[i]->get_concentration().val / min_concentration;
	
	if (ratio > zap_concentration_ratio)
	  {
	    cerr << "removing component " << i 
		 << " concentration=" << components[i]->get_concentration();
	    
	    components.erase( components.begin() + i );
	    component_removed = true;
	  }
	else
	  i++;
      }
    }
    
    if (component_removed)
    {
	cerr << "FIT INIT" << endl;
	model = 0;
	build ();
	chisq = fit.init (xval, yval, *model);
	cerr << "FIT INIT OK" << endl;
    }

    else if (nchisq < chisq)
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

  if (phase && verbose)
    cerr << "ComponentModel::fit phase=" << phase->get_value() << endl;
  
  std::vector<std::vector<double> > covar;
  fit.result (*model, covar);
  
  for (unsigned iparam=0; iparam < model->get_nparam(); iparam++)
  {
    double variance = covar[iparam][iparam];

    if (verbose)
      cerr << "ComponentModel::fit"
	" variance[" << iparam << "]=" << variance << endl;

    if (!isfinite(variance))
      throw Error (InvalidState, 
		   "ComponentModel::fit",
		   "non-finite variance "
		   + model->get_param_name(iparam));

    if (!model->get_infit(iparam) && variance != 0)
      throw Error (InvalidState,
		   "ComponentModel::fit",
		   "non-zero unfit variance "
		   + model->get_param_name(iparam)
		   + " = " + tostring(variance) );

    if (variance < 0)
      throw Error (InvalidState,
		   "ComponentModel::fit",
		   "invalid variance " + model->get_param_name(iparam)
		   + " = " + tostring(variance) );

    // cerr << iparam << ".var=" << variance << endl;

    model->set_variance (iparam, variance);
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
catch (Error& error)
{
    cerr << "ComponentModel::fit exception caught: " << error << endl;

    for (unsigned icomp=0; icomp < components.size(); icomp++)
	cerr << "\t" << icomp
	     << " centre=" << components[icomp]->get_centre() 
	     << " concentration=" << components[icomp]->get_concentration ()
	     << " height=" << components[icomp]->get_height() << endl;

    throw error += "ComponentModel::fit";
}

void ComponentModel::evaluate (float *vals,
				       unsigned nvals, int icomp_selected) 
{
  double phase_offset = 0;
  if (report_absolute_phases)
    phase_offset = phase->get_value().val;

  if (verbose)
    cerr << "ComponentModel::evaluate"
      " phase_offset=" << phase_offset/(2*M_PI) << " turns" << endl;
  
  //Construct the summed model
  SumRule< Univariate<Scalar> > m; 
  if (icomp_selected >= 0)
  {
    check ("evaluate", icomp_selected);
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
    argument.set_value( (i+0.5) * gate / nvals * 2*M_PI + phase_offset );
    vals[i] = m.evaluate();
  }
}

void ComponentModel::clear()
{
  components.clear();
  component_names.clear();
  comments.clear();
  model = 0;
}

// Text interface to the ComponentModel class
class Interface : public TextInterface::To<ComponentModel>
{
public:
  Interface (ComponentModel* = 0);
};

Interface::Interface (ComponentModel* instance)
{
  if (instance)
    set_instance (instance);

  add( &ComponentModel::get_zap_height_ratio,
       &ComponentModel::set_zap_height_ratio,
       "zhr", "Zap height ratio - remove small components");

  add( &ComponentModel::get_zap_concentration_ratio,
       &ComponentModel::set_zap_concentration_ratio,
       "zcr", "Zap concentration ratio - remove narrow components");
}

TextInterface::Parser* ComponentModel::get_interface ()
{
  return new Interface(this);
}
