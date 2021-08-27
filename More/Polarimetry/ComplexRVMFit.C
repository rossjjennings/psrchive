/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ComplexRVMFit.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/SmoothMedian.h"
#include "Pulsar/SmoothMean.h"
#include "Pulsar/MaskSmooth.h"

#include "MEAL/ComplexRVM.h"
#include "MEAL/RotatingVectorModel.h"
#include "MEAL/OrthoRVM.h"

#include "MEAL/ScalarParameter.h"
#include "MEAL/LevenbergMarquardt.h"

#include "templates.h"

//#define _DEBUG 1
#include "debug.h"

#include <cassert>
#include <fstream>

using namespace std;

bool Pulsar::ComplexRVMFit::verbose = false;

Pulsar::ComplexRVMFit::ComplexRVMFit()
{
  gate = 1.0;
  
  threshold = 3.0;
  chisq_map = false;
  
  range_alpha = range_beta = range_zeta = range (0,0);

  // favour impact angle smaller than colatitude of magnetic axis
  guess_alpha = 0.5;
  guess_beta = 0.25;
  guess_smooth = 3;

  auto_detect_opm = false;
}

void Pulsar::ComplexRVMFit::set_threshold (float sigma)
{
  threshold = sigma;
}

float Pulsar::ComplexRVMFit::get_threshold () const
{
  return threshold;
}

void Pulsar::ComplexRVMFit::set_guess_smooth (unsigned phase_bins)
{
  guess_smooth = phase_bins;
}

unsigned Pulsar::ComplexRVMFit::get_guess_smooth () const
{
  return guess_smooth;
}

template<class T>
bool notset (Reference::To<T>& model)
{
  return !model || (model->get_infit(0) && model->get_param(0) == 0.0);
}

template<typename T, typename U>
double chisq (const Estimate<T,U>& e)
{
  return e.val * e.val / e.var;
}

template<typename T, typename U>
double chisq (const std::complex< Estimate<T,U> >& z)
{
  return chisq(z.real()) + chisq(z.imag());
}

void Pulsar::ComplexRVMFit::init (MEAL::RotatingVectorModel* rvm)
{
  if (!rvm)
    return;

  cerr << "Pulsar::ComplexRVMFit::init MEAL::RotatingVectorModel" << endl;

  if (notset( rvm->magnetic_axis ))
    rvm->magnetic_axis->set_param (0, M_PI/2);
      
  if (notset( rvm->line_of_sight ) || notset( rvm->impact ))
  {
    cerr << "Pulsar::ComplexRVMFit::set_observation using"
      " delpsi_delphi=" << delpsi_delphi << endl;

    /*

    Choose a reasonable first guess for beta
    using Equation 5 of Everett & Weisberg (2001)

    */

    double alpha = rvm->magnetic_axis->get_param (0);
    
    double sin_alpha = sin (alpha);
    double sin_beta = - sin_alpha / delpsi_delphi;    
    double beta = asin( sin_beta );

    if (verbose)
      cerr << "Pulsar::ComplexRVMFit::set_observation"
	" alpha=" << alpha << " beta=" << beta << endl;

    if (rvm->impact)
      rvm->impact->set_param (0, beta);
    else
      rvm->line_of_sight->set_param (0, alpha+beta);
  }
}

void Pulsar::ComplexRVMFit::init (MEAL::OrthoRVM* rvm)
{
  if (!rvm)
    return;

  if (notset( rvm->inverse_slope ))
    rvm->inverse_slope->set_param (0, 1.0/delpsi_delphi);

  // start with most probable value as first guess
  if (notset( rvm->line_of_sight ))
    rvm->line_of_sight->set_param (0, M_PI/2);
  
  // rvm->line_of_sight->set_infit (0, false);
}

//! Set the data to which model will be fit
void Pulsar::ComplexRVMFit::set_observation (const PolnProfile* _data)
{
  if (_data->get_state() != Signal::Stokes)
    throw Error (InvalidParam, "Pulsar::ComplexRVMFit::set_observation"
		 "data are not Stokes parameters");

  data = _data;
  data_x.clear();
  data_y.clear();

  if (model && model->get_nstate())
    model = 0;

  if (verbose)
    cerr << "Pulsar::ComplexRVMFit::set_observation"
      " threshold=" << threshold << endl;

  data->get_linear (linear, threshold);
  find_delpsi_delphi_max();

  unsigned count = 0;
  max_L = 0;

  const unsigned nbin = data->get_nbin();
  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
    double phase = (ibin + 0.5) * gate * 2*M_PI / nbin;

    if (is_included(phase) ||
	(!is_excluded(phase) && linear[ibin].real().get_variance() > 0))
    {
      data_x.push_back ( state.get_Value(count) );
      data_y.push_back ( linear[ibin] );
      
      double L = sqrt( norm(linear[ibin]).val );

      if ( L > max_L )
	max_L = L;

      if (auto_detect_opm && is_opm(phase))
      {
	if (verbose)
	  cerr << "Pulsar::ComplexRVMFit::set_observation OPM at "
	       << phase << " rad" << endl;
	L *= -1.0;
      }

      if (verbose)
        cerr << "Pulsar::ComplexRVMFit::set_observation phase=" << phase 
             << " L=" << L << endl;

      get_model()->add_state (phase, L);

      count ++;
    }
  }

  assert( count == model->get_nstate() );

  if (verbose)
    cerr << "Pulsar::ComplexRVMFit::set_observation"
      " peak phase=" << peak_phase*180/M_PI << " deg;"
      " PA=" << peak_pa*180/M_PI << " deg;"
      " dpsi/dphi=" << delpsi_delphi << endl;
  
  MEAL::RVM* rvm = get_model()->get_rvm();

  if (notset( rvm->reference_position_angle ))
  {
    cerr << "Pulsar::ComplexRVMFit::set_observation using"
      " psi0=" << peak_pa*180/M_PI << endl;

    rvm->reference_position_angle->set_param (0, peak_pa);
  }

  if (notset( rvm->magnetic_meridian ))
  {
    cerr << "Pulsar::ComplexRVMFit::set_observation using"
      " phi0=" << peak_phase*180/M_PI << endl;

    rvm->magnetic_meridian->set_param (0, peak_phase);
  }

  init( dynamic_cast<MEAL::RotatingVectorModel*> (rvm) );
  init( dynamic_cast<MEAL::OrthoRVM*> (rvm) );
  
  state.signal.connect (model, &MEAL::ComplexRVM::set_state);

  if (!auto_detect_opm)
    return;

  // search for OPMs

  unsigned nstate = model->get_nstate();

  for (unsigned istate=0; istate < nstate; istate++)
  {
    data_x[istate].apply ();
    complex<double> Lmodel = get_model()->evaluate();
    
    double chisq_pos = ::chisq (data_y[istate] - Lmodel);
    double chisq_neg = ::chisq (data_y[istate] + Lmodel);

    if (chisq_neg < chisq_pos)
    {
      MEAL::ComplexRVM* cRVM = get_model();

      cerr << "Pulsar::ComplexRVMFit::set_observation OPM"
	" istate=" << istate <<
	" phase=" << cRVM->get_phase(istate)*180/M_PI << " deg"
	   << endl;

      cRVM->set_linear(istate, -cRVM->get_linear(istate).get_value());

      cerr << "new linear=" << cRVM->get_linear(istate) << endl;
    }
  }

  cerr << "Pulsar::ComplexRVMFit::set_observation " 
       << count << " bins" << endl;
}


void Pulsar::ComplexRVMFit::find_delpsi_delphi_max ()
{
  const unsigned nbin = linear.size();

  delpsi_delphi = 0;

  // the Profile class initializes the amps array = zero
  Profile real (nbin);
  float* re = real.get_amps();
  Profile imag (nbin);
  float* im = imag.get_amps();

  std::complex< Estimate<double> > cross;

  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
    cross = std::conj(linear[ibin]) * linear[ (ibin+1) % nbin ];
    
    re[ibin] = cross.real().get_value();
    im[ibin] = cross.imag().get_value();
  }

#if _DEBUG
  {
    std::ofstream out ("reim.txt");
    for (unsigned ibin=0; ibin < nbin; ibin++)
      out << re[ibin] << " " << im[ibin] << endl;
  }
#endif
  
  PhaseWeight mask (nbin, 1.0);
  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
    if ( linear[ibin].real().get_error() == 0.0 )
      mask[ibin] = 0.0;

    double phase = (ibin + 0.5) * gate * 2*M_PI / nbin;
    if (is_excluded(phase) && !is_included(phase))
      mask[ibin] = 0.0;

#if _DEBUG
    cerr << "init mask " << ibin << " " << mask[ibin] << endl;
#endif
  }

  
  if (guess_smooth)
  {
    {
      SmoothMean smooth;
      smooth.set_bins (guess_smooth);
      
      smooth (&real);
      smooth (&imag);
    }

    {
      MaskSmooth smooth;
      smooth.set_bins (guess_smooth);
      smooth.set_masked_bins (1);

      smooth (&mask);
    }
  }

#if _DEBUG
  for (unsigned ibin=0; ibin < nbin; ibin++)
    cerr << "smimag: " << ibin << " " << im[ibin] << endl;

  std::ofstream out ("reimphi.txt");  
#endif

  int max_bin = -1;
  double max_angle = 0.0;

  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
    if (mask[ibin] == 0)
      continue;
    
    double angle = fabs( atan2 (im[ibin], re[ibin]) );
    
#if _DEBUG
    out << ibin << " " << im[ibin] << " " << re[ibin] << " " << angle << endl;
    cerr << "angle: " << ibin << " " << angle
	 << " " << im[ibin] << " " << re[ibin] << " " << mask[ibin] << endl;
#endif
    
    if (max_bin < 0 || angle > max_angle) 
    {
      max_bin = ibin;
      max_angle = angle;
    }
  }

  double phi_per_bin = gate * 2*M_PI / nbin;

  peak_phase = (max_bin + 0.5) * phi_per_bin;
  delpsi_delphi = 0.5 * atan2(im[max_bin],re[max_bin]) / phi_per_bin;
  
  std::complex< Estimate<double> > L0 = linear[max_bin];
  peak_pa = 0.5 * atan2(L0.imag().get_value(), L0.real().get_value());

#if _DEBUG
  cerr << "ComplexRVMFit::find_delpsi_delphi_max gate=" << gate
       << " phi_per_bin=" << phi_per_bin
       << "\n\t delpsi_delphi=" << delpsi_delphi
       << "\n\t peak_phase=" << peak_phase*180/M_PI << " deg"
       << "\n\t peak_pa=" << peak_pa*180/M_PI << " deg" << endl;
#endif
}
				      

void add_radian_range (std::vector<range>& data, range r)
{
  if (r.first > r.second)
    r.second += 2*M_PI;

  data.push_back (r);
}

bool between (const range& r, double p)
{
  return p > r.first && p < r.second;
}

bool is_radian_range (const std::vector<range>& data, double phase)
{
  for (unsigned i=0; i<data.size(); i++)
    if (between(data[i],phase) || between(data[i],phase+2*M_PI))
      return true;
  return false;
}

void Pulsar::ComplexRVMFit::add_opm (const range& r)
{
  add_radian_range (opm, r);
}

bool Pulsar::ComplexRVMFit::is_opm (double phase) const
{
  return is_radian_range (opm, phase);
}

void Pulsar::ComplexRVMFit::add_include (const range& r)
{
  add_radian_range (range_include, r);
}

bool Pulsar::ComplexRVMFit::is_included (double phase) const
{
  return is_radian_range (range_include, phase);
}

void Pulsar::ComplexRVMFit::add_exclude (const range& r)
{
  add_radian_range (range_exclude, r);
}

bool Pulsar::ComplexRVMFit::is_excluded (double phase) const
{
  return is_radian_range (range_exclude, phase);
}

//! Get the data to which model will be fit
const Pulsar::PolnProfile* Pulsar::ComplexRVMFit::get_observation () const try
{
  return data;
}
catch (Error& error)
{
  throw error += "Pulsar::ComplexRVMFit::get_observation";
}

//! Get the model to be fit to the data
MEAL::ComplexRVM* Pulsar::ComplexRVMFit::get_model ()
{
  if (!model)
    model = new MEAL::ComplexRVM;

  return model;
}

//! Fit data to the model
void Pulsar::ComplexRVMFit::solve ()
{
  
  if (!model)
    model = new MEAL::ComplexRVM;

  MEAL::LevenbergMarquardt< complex<double> > fit;
  fit.verbose = MEAL::Function::verbose;

  chisq = fit.init (data_x, data_y, *model);

  if (!isfinite(chisq))
    throw Error (InvalidState, "Pulsar::ComplexRVMFit::solve",
		 "non-finite chisq");

  if (verbose)
    cerr << "Pulsar::ComplexRVMFit::solve initial chisq = " << chisq << endl;

  float close = 1e-3;
  unsigned iter = 1;
  unsigned not_improving = 0;
  while (not_improving < 25)
  {
    if (verbose)
      cerr << "Pulsar::ComplexRVMFit::solve iteration " << iter << endl;

    float nchisq = fit.iter (data_x, data_y, *model);

    if (verbose)
      cerr << "     chisq = " << nchisq << endl;

    if (!isfinite (nchisq))
      throw Error (InvalidState, "Pulsar::ComplexRVMFit::solve",
		   "non-finite chisq");

    if (nchisq < chisq)
    {
      float diffchisq = chisq - nchisq;
      chisq = nchisq;
      not_improving = 0;

      if (diffchisq/chisq < close && diffchisq > 0)
	break;
    }
    else
      not_improving ++;

    iter ++;
  }

  if (verbose)
    cerr << "Pulsar::ComplexRVMFit::solve iterations = " << iter << endl;

  check_parameters ();

  if (chisq_map)
    return;
  
  std::vector<std::vector<double> > covariance;
  fit.result (*model, covariance);

  nfree = 2 * model->get_nstate();

  cerr << "ComplexRVMFit::solve nconstraint=" << nfree << endl;

  for (unsigned iparm=0; iparm < model->get_nparam(); iparm++)
  {
    if (model->get_infit(iparm))
      nfree --;

    if (nfree == 0)
      throw Error (InvalidState, "Pulsar::ComplexRVMFit::solve",
		   "nfree <= 0");

    model->set_variance (iparm, 2.0*covariance[iparm][iparm]);
  }

  cerr << "ComplexRVMFit::solve nfree=" << nfree << endl;

}

// ensure that phi lies on 0 -> 2*PI
double twopi (double phi)
{
  const double s = 2*M_PI;
  phi /= s;
  return s*(phi-floor(phi));
}


void Pulsar::ComplexRVMFit::check_parameters (MEAL::RotatingVectorModel* rvm)
{
  if (!rvm)
    return;
  
  double PA0 = rvm->reference_position_angle->get_param(0);
  double phi0 = rvm->magnetic_meridian->get_param(0);

  double alpha = rvm->magnetic_axis->get_param(0);
  double zeta = 0.0;
  if (rvm->impact)
    zeta = rvm->impact->get_param(0) + alpha;
  else
    zeta = rvm->line_of_sight->get_param(0);

  if (verbose)
    cerr << "Pulsar::ComplexRVMFit::check_parameters initial result:\n"
            "  PA_0=" << PA0*180/M_PI << " deg \n"
            "  zeta=" << zeta*180/M_PI << " deg \n"
            "  alpha=" << alpha*180/M_PI << " deg \n"
            "  phi0=" << phi0*180/M_PI << " deg" << endl;

  // ensure that alpha lies on 0 -> pi
  alpha = twopi (alpha);
  if (alpha > M_PI)
  {
    if (verbose)
      cerr << "Pulsar::ComplexRVMFit::check_parameters alpha -= pi" << endl;

    alpha -= M_PI;
  }

  // ensure that zeta lies on 0 -> pi
  zeta = twopi (zeta);
  if (zeta > M_PI)
  {
    if (verbose)
      cerr << "Pulsar::ComplexRVMFit::check_parameters zeta=2pi-zeta" << endl;

    zeta = 2*M_PI - zeta;
    phi0 -= M_PI;
  }

  DEBUG("alpha=" << alpha << " zeta=" << zeta << " neg=" << negative_count);

  // ensure that phi0 lies on 0 -> 2pi
  phi0 = twopi (phi0);

  rvm->magnetic_axis->set_param(0, alpha);
  if (rvm->impact)
    rvm->impact->set_param(0, zeta - alpha);
  else
    rvm->line_of_sight->set_param(0, zeta);
  rvm->magnetic_meridian->set_param(0, phi0);
}

void Pulsar::ComplexRVMFit::check_parameters ()
{
  MEAL::ComplexRVM* cRVM = get_model();

  const unsigned nstate = cRVM->get_nstate();

  double total_linear = 0.0;
  unsigned negative_count = 0;
  for (unsigned i=0; i<nstate; i++)
  {
    if (verbose)
      cerr << "Pulsar::ComplexRVMFit::check_parameters L[" << i << "]="
           << cRVM->get_linear(i).get_value() << endl;

    total_linear += cRVM->get_linear(i).get_value();

    if (cRVM->get_linear(i).get_value() <= 0)
      negative_count ++;
  }

  MEAL::RVM* rvm = cRVM->get_rvm();
  double PA0 = rvm->reference_position_angle->get_param(0);
  
  if (total_linear < 0.0)
  {
    if (verbose)
      cerr << "Pulsar::ComplexRVMFit::check_parameters negative gain" << endl;

    for (unsigned i=0; i<nstate; i++)
      cRVM->set_linear(i,-cRVM->get_linear(i).get_value());

    // shift the reference P.A. by 90 degrees
    PA0 += M_PI/2;
  }

  // ensure that PA0 lies on -pi/2 -> pi/2
  PA0 = atan ( tan(PA0) );
  rvm->reference_position_angle->set_param (0, PA0);
  
  check_parameters ( dynamic_cast<MEAL::RotatingVectorModel*> (rvm) );
}

double Pulsar::ComplexRVMFit::evaluate (double rad)
{
  double offset = (is_opm(rad)) ? M_PI/2 : 0;
  return get_model()->get_rvm()->compute(rad) + offset;
}

void Pulsar::ComplexRVMFit::get_residuals 
( vector<double>& phases,
  vector< std::complex< Estimate<double> > >& residuals)
{
  MEAL::ComplexRVM* cRVM = get_model();

  unsigned ndat = data_x.size();

  phases.resize (ndat);
  residuals.resize (ndat);

  for (unsigned i=0; i < ndat; i++)
  {
    data_x[i].apply();
    std::complex<double> model = cRVM->evaluate();
    phases[i] = cRVM->get_phase(i);
    residuals[i] = data_y[i];
    residuals[i] -= model;
  }
}

void Pulsar::ComplexRVMFit::get_psi_residuals 
( vector<double>& phases,
  vector< Estimate<double> > & residuals)
{
  MEAL::ComplexRVM* cRVM = get_model();

  unsigned ndat = data_x.size();

  phases.resize (ndat);
  residuals.resize (ndat);

  for (unsigned i=0; i < ndat; i++)
  {
    data_x[i].apply();
    std::complex<double> model = cRVM->evaluate();
    phases[i] = cRVM->get_phase(i);

    residuals[i] = 0.5 * atan2( data_y[i].imag(), data_y[i].real() );
    residuals[i] -= 0.5 * atan2( model.imag(), model.real() );

    if (residuals[i].val > M_PI/2)
      residuals[i].val -= M_PI;
    else if (residuals[i].val < -M_PI/2)
      residuals[i].val += M_PI;
  }
}



void Pulsar::ComplexRVMFit::search_2D (unsigned nalpha, unsigned nzeta)
{
  MEAL::ComplexRVM* cRVM = get_model();
  MEAL::RotatingVectorModel* rvm = 0;

  rvm = dynamic_cast<MEAL::RotatingVectorModel*> (cRVM->get_rvm());

  if (!rvm)
    throw Error (InvalidState, "Pulsar::ComplexRVMFit::global_search",
		 "not implemented for orthometric RVM");

  const unsigned nstate = cRVM->get_nstate();
  if (!nstate)
    throw Error (InvalidState, "Pulsar::ComplexRVMFit::global_search",
		 "no data");

  vector<double> linear (nstate);
  vector<double> best_linear (nstate);

  for (unsigned i=0; i<nstate; i++)
    best_linear[i] = linear[i] = cRVM->get_linear(i).get_value();
  
  bool map_beta = range_beta.first != range_beta.second;
  if (map_beta)
    range_zeta = range_beta;

  rvm->use_impact (map_beta);

  double step_alpha = M_PI/nalpha;
  double step_zeta = M_PI/nzeta;

  if ( range_alpha.first == range_alpha.second )
  {
    range_alpha.first = step_alpha/2;
    range_alpha.second = M_PI;
  }
  else
  {
    step_alpha = (range_alpha.second - range_alpha.first) / (nalpha - 1);
    // ensure that for loop ends on range_alpha.second within rounding error
    range_alpha.second += step_alpha / 2;
  }

  if ( range_zeta.first == range_zeta.second )
  {
    range_zeta.first = step_zeta/2;
    range_zeta.second = M_PI;
  }
  else
  {
    step_zeta = (range_zeta.second - range_zeta.first) / (nzeta - 1);
    // ensure that for loop ends on range_zeta.second within rounding error
    range_zeta.second += step_zeta / 2;
  }

  float best_chisq = 0.0;
  float best_alpha = 0.0;
  float best_zeta = 0.0;
  
  float best_phi0 = peak_phase;
  float best_psi0 = peak_pa;
  
  vector<double> chisq_surface;
  unsigned chisq_index = 0;

  if (chisq_map)
  {
    // disable fits for alpha and zeta/beta
    rvm->magnetic_axis->set_infit (0, false);
    if (rvm->impact)
      rvm->impact->set_infit (0, false);
    else
      rvm->line_of_sight->set_infit (0, false);

    chisq_surface.resize (nalpha * nzeta);
  }

  for (double alpha=range_alpha.first; 
       alpha <= range_alpha.second; 
       alpha += step_alpha)
  {
    for (double zeta=range_zeta.first; 
	 zeta <= range_zeta.second;
	 zeta += step_zeta) try
    {
      if (alpha == zeta)
      {
        if (chisq_map)
        {
          assert (chisq_index < chisq_surface.size());
          chisq_surface[chisq_index] = 0.0;
          chisq_index ++;
        }
	continue;
      }

      cerr << "alpha=" << alpha;
      rvm->magnetic_axis->set_value (alpha);

      if (map_beta)
      {
	cerr << " beta=" << zeta << endl;
	rvm->impact->set_value (zeta);
      }
      else
      {
	cerr << " zeta=" << zeta << endl;
	rvm->line_of_sight->set_value (zeta);
      }

      // ensure that each attempt starts with the same guess
#if 1
      rvm->magnetic_meridian->set_value (peak_phase);
      rvm->reference_position_angle->set_value (peak_pa);
      for (unsigned i=0; i<nstate; i++)
	cRVM->set_linear(i, linear[i]);
#else
      // start each attempt with the values from the best fit
      rvm->magnetic_meridian->set_value (best_phi0);
      rvm->reference_position_angle->set_value (best_psi0);
      for (unsigned i=0; i<nstate; i++)
	cRVM->set_linear(i, best_linear[i]);
#endif

      solve ();

      if (chisq_map)
      {
	assert (chisq_index < chisq_surface.size());
	chisq_surface[chisq_index] = chisq;
        chisq_index ++;
      }

      if (best_chisq == 0 || chisq < best_chisq)
      {
	// cerr << "current best chisq=" << chisq << endl;
	best_chisq = chisq;
	best_alpha = alpha;
	best_zeta = zeta;

	best_phi0 = rvm->magnetic_meridian->get_value ().val;
	best_psi0 = rvm->reference_position_angle->get_value ().val;
	for (unsigned i=0; i<nstate; i++)
	  best_linear[i] = cRVM->get_linear(i).get_value();
      }
    }
    catch (Error& error)
    {
      cerr << "fit failed on alpha=" << alpha << " zeta=" << zeta << endl;
      if (verbose)
	cerr << error.get_message() << endl;

      chisq_index ++;
    }
  }

  if (chisq_map)
  {
    assert (chisq_index == chisq_surface.size());
    double min=0, max=0;
    minmax (chisq_surface, min, max);
    for (unsigned i=0; i<chisq_index; i++)
      chisq_surface[i] -= min;

    chisq_index = 0;

    for (double alpha=range_alpha.first; 
	 alpha < range_alpha.second; 
	 alpha += step_alpha)
    {
      for (double zeta=range_zeta.first; 
	   zeta < range_zeta.second;
	   zeta += step_zeta)
      {
	const double deg = 180/M_PI;
	cout << alpha*deg << " " << zeta*deg 
	     << " " << chisq_surface[chisq_index] << endl;
	chisq_index ++;
      }
      cout << endl;
    }
  }

  cerr << "BEST chisq=" << best_chisq << endl;

  rvm->magnetic_axis->set_value (best_alpha);

  if (map_beta)
    rvm->impact->set_value (best_zeta);
  else
    rvm->line_of_sight->set_value (best_zeta);

  // ensure that each attempt starts with the same guess
  rvm->magnetic_meridian->set_value (peak_phase);
  rvm->reference_position_angle->set_value (peak_pa);
  for (unsigned i=0; i<nstate; i++)
    cRVM->set_linear(i, linear[i]);

  chisq_map = false;

  solve ();

  for (unsigned i=0; i<nstate; i++)
    if (cRVM->get_linear(i).get_value() < 0)
      cerr << "orthogonal mode L=" << cRVM->get_linear(i).val 
	   << " phase=" << cRVM->get_phase(i)*0.5/M_PI  << endl;
}


void Pulsar::ComplexRVMFit::search_1D (unsigned nsearch)
{
  MEAL::ComplexRVM* cRVM = get_model();
  MEAL::RVM* rvm = cRVM->get_rvm();
  
  Reference::To<MEAL::ScalarParameter> search;
  Reference::To<MEAL::ScalarParameter> other;

  MEAL::RotatingVectorModel* orig_rvm = 0;
  orig_rvm = dynamic_cast<MEAL::RotatingVectorModel*> (rvm);
  if (orig_rvm)
  {
    cerr << "original RVM a" << endl;
    search = orig_rvm->magnetic_axis;
    cerr << "original RVM b" << endl;
    orig_rvm->use_impact();
    other = orig_rvm->impact;
    cerr << "original RVM c" << endl;
  }

  MEAL::OrthoRVM* ortho_rvm = 0;
  ortho_rvm = dynamic_cast<MEAL::OrthoRVM*> (rvm);
  if (ortho_rvm)
  {
    search = ortho_rvm->line_of_sight;
    other = ortho_rvm->inverse_slope;
  }
  
  const unsigned nstate = cRVM->get_nstate();
  if (!nstate)
    throw Error (InvalidState, "Pulsar::ComplexRVMFit::global_search",
		 "no data");

  vector<double> linear (nstate);
  vector<double> best_linear (nstate);

  for (unsigned i=0; i<nstate; i++)
    best_linear[i] = linear[i] = cRVM->get_linear(i).get_value();

  double step_search = -M_PI/nsearch;

  if ( range_zeta.first == range_zeta.second )
  {
    range_zeta.first = M_PI + step_search/4;
    range_zeta.second = -step_search/4;
  }
  else
  {
    step_search = (range_zeta.second - range_zeta.first) / (nsearch - 1);
    // ensure that for loop ends on range_zeta.second within rounding error
    range_zeta.second += step_search / 2;
  }

  float best_chisq = 0.0;

  float best_search = 0.0;
  float best_other = 0.0;  
  float best_phi0 = peak_phase;
  float best_psi0 = peak_pa;
  
  vector<double> chisq_surface;
  unsigned chisq_index = 0;

  if (chisq_map)
  {
    search->set_infit (0, false);
    chisq_surface.resize (nsearch);
  }

  for (double current_search=range_zeta.first; 
       current_search > range_zeta.second;
       current_search += step_search) try
  {
    cerr << " search=" << current_search << endl;
    search->set_value (current_search);

#if 1
    // ensure that each attempt starts with the same guess
    rvm->magnetic_meridian->set_value (peak_phase);
    rvm->reference_position_angle->set_value (peak_pa);
    for (unsigned i=0; i<nstate; i++)
      cRVM->set_linear(i, linear[i]);
#endif
    
    solve ();
    
    if (chisq_map)
    {
      cerr <<chisq_index<< " " << chisq_surface.size()<<endl;
      assert (chisq_index < chisq_surface.size());
      chisq_surface[chisq_index] = chisq;
      chisq_index ++;
    }
    
    if (best_chisq == 0 || chisq < best_chisq)
    {
      // cerr << "current best chisq=" << chisq << endl;
      best_chisq = chisq;
      best_search = current_search;

      best_other = other->get_value ().val;
      best_phi0 = rvm->magnetic_meridian->get_value ().val;
      best_psi0 = rvm->reference_position_angle->get_value ().val;
      for (unsigned i=0; i<nstate; i++)
	best_linear[i] = cRVM->get_linear(i).get_value();
    }
  }
  catch (Error& error)
  {
    cerr << "fit failed on search=" << current_search << endl;
    if (verbose)
      cerr << error.get_message() << endl;

    chisq_index ++;
  }

  if (chisq_map)
  {
    assert (chisq_index == chisq_surface.size());
    double min=0, max=0;
    minmax (chisq_surface, min, max);

    cerr << "min=" << min << endl;
    
    for (unsigned i=0; i<chisq_index; i++)
      chisq_surface[i] -= min;

    chisq_index = 0;
    
    for (double current_search=range_zeta.first; 
	 current_search > range_zeta.second;
	 current_search += step_search)
    {
      const double deg = 180/M_PI;
      cout << current_search*deg 
	   << " " << chisq_surface[chisq_index] << endl;
      chisq_index ++;
    }
  }

  cerr << "BEST chisq=" << best_chisq << endl;

  search->set_value (best_search);
  other->set_value (best_other);
  
  // ensure that each attempt starts with the same guess
  rvm->magnetic_meridian->set_value (peak_phase);
  rvm->reference_position_angle->set_value (peak_pa);
  for (unsigned i=0; i<nstate; i++)
    cRVM->set_linear(i, linear[i]);

  chisq_map = false;

  solve ();

  for (unsigned i=0; i<nstate; i++)
    if (cRVM->get_linear(i).get_value() < 0)
      cerr << "orthogonal mode L=" << cRVM->get_linear(i).val 
	   << " phase=" << cRVM->get_phase(i)*0.5/M_PI  << endl;
}
