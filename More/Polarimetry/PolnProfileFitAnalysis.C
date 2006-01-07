#include "Pulsar/PolnProfileFitAnalysis.h"
#include "Pulsar/PolnProfile.h"

#include "Calibration/ReceptionModel.h"

#include "Pauli.h"

using namespace std;

Pulsar::PolnProfileFitAnalysis::PolnProfileFitAnalysis ()
{
  compute_error = true;
}

//! When set, estimate the uncertainty in each attribute
void Pulsar::PolnProfileFitAnalysis::set_compute_error (bool flag)
{
  compute_error = flag;
}

void
Pulsar::PolnProfileFitAnalysis::set_harmonic (unsigned index)
{
  double phase_shift = -2.0 * M_PI * double(index+1);

  fit->phase_axis.set_value (phase_shift);
  fit->model->set_input_index (index);
}

void
Pulsar::PolnProfileFitAnalysis::get_curvature (Matrix<8,8,double>& curvature)
{
  curvature = Matrix<8,8,double>();

  for (unsigned ih=0; ih < fit->model->get_num_input(); ih++) {

    set_harmonic (ih);
    model_result = fit->model->evaluate (&model_gradient);

#ifdef _DEBUG
    for (unsigned ig=0; ig < model_gradient.size(); ig++)
      cerr << "g[" << fit->model->get_param_name(ig) << "]=\t"
	   << model_gradient[ig] << endl;
#endif

    for (unsigned ir=0; ir < 8; ir++) {

      Jones<double> delrho_deleta_r = model_gradient[ir+2];    

      for (unsigned is=0; is < 8; is ++) {

	Jones<double> delrho_deleta_s = model_gradient[is+2];

	curvature[ir][is]+=trace(delrho_deleta_r*herm(delrho_deleta_s)).real();
      }

    }

  }

  mean_variance = 0.0;

  // FACTOR OF TWO INCREASE IN VARIANCE - SEE OLD VERSION OF VAN STRATEN 2004
  for (unsigned i=0; i<4; i++)
    mean_variance += fit->standard_variance[i] / 2.0;

  // mean_variance = 2.0;

  curvature /= mean_variance;
}

void conformal_partition (const Matrix<8,8,double>& covariance,
			  double& c_varphi,
			  Vector <7,double>& C_varphiJ,
			  Matrix <7,7,double>& C_JJ)
{
  c_varphi = covariance[0][0];

#define OUTPUT_COVARIANCE 0

#if OUTPUT_COVARIANCE
  fprintf (stderr, "%12s%12.3g\n", "phase", covariance[0][0]);
#endif

  for (unsigned i=0; i < 7; i++) {

    C_varphiJ[i] = covariance[i+1][0];

#if OUTPUT_COVARIANCE
    unsigned m = i + 3;
    fprintf (stderr, "%12s", fit->model->get_param_name(m).c_str());
    fprintf (stderr, "%12.3g", C_varphiJ[i]);
#endif

    for (unsigned j=0; j < 7; j++) {

      C_JJ[i][j] = covariance[i+1][j+1];

      if (j > i)
	continue;

#if OUTPUT_COVARIANCE
      fprintf (stderr, "%12.3g", C_JJ[i][j]);
#endif

    }

#if OUTPUT_COVARIANCE
    fprintf (stderr, "\n");
#endif
  }

}

/*! Return the partial derivative of the multiple correlation squared with
  respect to the real or imaginary component of a Stokes parameter */

double 
Pulsar::PolnProfileFitAnalysis::delR2_varphiJ_delS 
(Matrix<8,8,double>& delC_delS) 
{
  // partial derivatives of conformal partition of covariance matrix
  Matrix <7,7,double> delC_JJ_delS;
  Vector <7,double> delC_varphiJ_delS;
  double delc_varphi_delS = 0;
      
  // partition the partial derivative of the covariance matrix
  conformal_partition (delC_delS, delc_varphi_delS,
		       delC_varphiJ_delS, delC_JJ_delS);

  return
    delC_varphiJ_delS * (inv_C_JJ * C_varphiJ) / c_varphi
    - C_varphiJ * (inv_C_JJ * delC_JJ_delS * inv_C_JJ * C_varphiJ) / c_varphi 
    + C_varphiJ * (inv_C_JJ * delC_varphiJ_delS) / c_varphi +
    - R2_varphiJ * delc_varphi_delS / c_varphi;
}

Jones<double> 
Pulsar::PolnProfileFitAnalysis::delgradient_delS (unsigned i, unsigned k) const
{
  Quaternion<double,Hermitian> q;
  q[k] = 1.0;
  Jones<double> sigma_k = convert (q);

  if (i == 0)
    return 0.5 * xform_result *sigma_k* herm(xform_result) * phase_gradient[2];

  i --;

  return 0.5 *
    ( xform_gradient[i] * sigma_k * herm(xform_result) * phase_result +
      xform_result * sigma_k * herm(xform_gradient[i]) * phase_result );
}

void Pulsar::PolnProfileFitAnalysis::delC_delS
( Matrix<8,8,double>& delC_delSre,
  Matrix<8,8,double>& delC_delSim, unsigned k ) const
{
  Matrix<8,8,double> delalpha_delSre;
  Matrix<8,8,double> delalpha_delSim;

  // over all rows
  for (unsigned ir=0; ir < 8; ir++) {
	
    Jones<double> del2rho_deleta_r = delgradient_delS (ir, k);
    Jones<double> delrho_deleta_r = model_gradient[ir+2];    

    // over all columns
    for (unsigned is=0; is < 8; is ++) {
	  
      Jones<double> del2rho_deleta_s = delgradient_delS (is, k);
      Jones<double> delrho_deleta_s = model_gradient[is+2];
	  
      delalpha_delSre[ir][is] = 
	trace( del2rho_deleta_r * herm(delrho_deleta_s) +
	       delrho_deleta_r  * herm(del2rho_deleta_s) ).real();

      complex<double> i (0,1);

      delalpha_delSim[ir][is] = 
	trace( i*del2rho_deleta_r * herm(delrho_deleta_s) +
	       delrho_deleta_r    * herm(i*del2rho_deleta_s) ).real();

    }
    
  }

  delalpha_delSre /= mean_variance;
  delalpha_delSim /= mean_variance;
  
  delC_delSre = -covariance * delalpha_delSre * covariance;
  delC_delSim = -covariance * delalpha_delSim * covariance;
}

//! Set the PolnProfileFit algorithm to be analysed
void Pulsar::PolnProfileFitAnalysis::set_fit (PolnProfileFit* f)
{
  fit = f;

  if (!fit)
    return;

  Matrix<8,8,double> curvature;

  // calculate the curvature matrix
  get_curvature (curvature);

  // calculate the covariance matrix
  covariance = inv(curvature);

  //cerr << "curv=\n" << curvature << endl;
  //cerr << "cov=\n" << covariance << endl;

  // partition the covariance matrix
  conformal_partition (covariance, c_varphi, C_varphiJ, C_JJ);

  // calculate the inverse of the Jones parameter covariance matrix
  inv_C_JJ = inv(C_JJ);

  // the multiple correlation squared
  R2_varphiJ = C_varphiJ * (inv_C_JJ * C_varphiJ) / c_varphi;

  // /////////////////////////////////////////////////////////////////////
  //
  // now do the same for scalar template matching
  //
  // /////////////////////////////////////////////////////////////////////

  ScalarProfileFitAnalysis scalar;
  scalar.set_fit (fit);

  Matrix<2,2,double> I_curvature;
  scalar.get_curvature (I_curvature);

  Matrix<2,2,double> I_covariance = inv(I_curvature);

  //cerr << "I_curv=\n" << I_curvature << endl;
  //cerr << "I_cov=\n" << I_covariance << endl;
    
  // the relative unconditional phase shift variance
  double hatvar_varphi = covariance[0][0] / I_covariance[0][0];

  // the relative conditional phase shift variance
  double hatvar_varphiJ = hatvar_varphi * (1-R2_varphiJ);

  if (!compute_error) {
    multiple_correlation = sqrt(R2_varphiJ);
    relative_error = sqrt(hatvar_varphi);
    relative_conditional_error = sqrt(hatvar_varphiJ);
    return;
  }

  // the variance of the relative unconditional phase shift variance
  double var_hatvar_varphi = 0.0;

  // the variance of the multiple correlation squared
  double var_R2_varphiJ = 0.0;

  // the variance of the relative conditional phase shift variance
  double var_hatvar_varphiJ = 0.0;

  // the variance of the phase shift variance
  double var_c_varphi = 0.0;

  for (unsigned ih=0; ih < fit->model->get_num_input(); ih++) {

    set_harmonic (ih);

    model_result = fit->model->evaluate (&model_gradient);
    xform_result = fit->transformation->evaluate (&xform_gradient);
    phase_result = fit->phase_xform->evaluate (&phase_gradient);

    // over all four stokes parameters
    for (unsigned ip=0; ip < 4; ip++) {

      // calculate the partial derivative of the covariance matrix wrt S_ip
      Matrix<8,8,double> delC_delSre;
      Matrix<8,8,double> delC_delSim;
      delC_delS (delC_delSre, delC_delSim, ip);

      // the variance of the real and imaginary parts of the Stokes parameters
      double var_S = 0.5*mean_variance;

      var_c_varphi += delC_delSre[0][0]*delC_delSre[0][0] * var_S;
      var_c_varphi += delC_delSim[0][0]*delC_delSim[0][0] * var_S;

      double delR2_varphiJ_delSre = delR2_varphiJ_delS (delC_delSre);
      double delR2_varphiJ_delSim = delR2_varphiJ_delS (delC_delSim);

      var_R2_varphiJ += delR2_varphiJ_delSre * delR2_varphiJ_delSre * var_S;
      var_R2_varphiJ += delR2_varphiJ_delSim * delR2_varphiJ_delSim * var_S;

#ifdef _DEBUG
      if (ip==3) {
	Estimate<double> R2 (R2_varphiJ, var_R2_varphiJ);
	multiple_correlation = sqrt(R2);

	cerr << ih << " R2 " << multiple_correlation.get_error() << endl;
      }
#endif

      double delA_delSre = hatvar_varphi * delC_delSre[0][0]/covariance[0][0];
      double delA_delSim = hatvar_varphi * delC_delSim[0][0]/covariance[0][0];

      if (ip == 0) {

	Matrix<2,2,double> delC_delS0re;
	Matrix<2,2,double> delC_delS0im;

	scalar.delC_delS (delC_delS0re, delC_delS0im, ih);

	delA_delSre -= hatvar_varphi * delC_delS0re[0][0]/I_covariance[0][0];
	delA_delSim -= hatvar_varphi * delC_delS0im[0][0]/I_covariance[0][0];

      }

      var_hatvar_varphi += delA_delSre * delA_delSre * var_S;
      var_hatvar_varphi += delA_delSim * delA_delSim * var_S;

      // ... and the conditional variance
      double delB_delSre = 
	delA_delSre * (1-R2_varphiJ) - hatvar_varphi * delR2_varphiJ_delSre;
      double delB_delSim = 
	delA_delSim * (1-R2_varphiJ) - hatvar_varphi * delR2_varphiJ_delSim;

      var_hatvar_varphiJ += delB_delSre * delB_delSre * var_S;
      var_hatvar_varphiJ += delB_delSim * delB_delSim * var_S;

    }

  }

  Estimate<double> R2 (R2_varphiJ, var_R2_varphiJ);
  multiple_correlation = sqrt(R2);

  Estimate<double> hatvar (hatvar_varphi, var_hatvar_varphi);
  relative_error = sqrt(hatvar);

  Estimate<double> hatvarJ (hatvar_varphiJ, var_hatvar_varphiJ);
  relative_conditional_error = sqrt(hatvarJ);

}

Estimate<double>
Pulsar::PolnProfileFitAnalysis::get_relative_error () const
{
  return relative_error;
}


Estimate<double>
Pulsar::PolnProfileFitAnalysis::get_multiple_correlation () const
{
  return multiple_correlation;
}

Estimate<double>
Pulsar::PolnProfileFitAnalysis::get_relative_conditional_error () const
{
  return relative_conditional_error;
}

// /////////////////////////////////////////////////////////////////////
//
// the same for scalar template matching
//
// /////////////////////////////////////////////////////////////////////

void Pulsar::ScalarProfileFitAnalysis::set_fit (const PolnProfileFit* fit)
{
  set_spectrum( fit->standard_fourier->get_Profile(0) );
  set_max_harmonic( fit->model->get_num_input() );

  mean_variance = 0.0;

  for (unsigned i=0; i<4; i++)
    mean_variance += fit->standard_variance[i] / 4.0;

  // mean_variance = 1.0;
}

void Pulsar::ScalarProfileFitAnalysis::set_spectrum (const Profile* p)
{
  spectrum = p;
  amps = reinterpret_cast<const complex<float>*> (p->get_amps() + 2);
}

void Pulsar::ScalarProfileFitAnalysis::set_max_harmonic (unsigned n)
{
  max_harmonic = n;
}

void Pulsar::ScalarProfileFitAnalysis::set_variance (double v)
{
  mean_variance = v;
}


void
Pulsar::ScalarProfileFitAnalysis::get_curvature (Matrix<2,2,double>& curvature)
{
  curvature = Matrix<2,2,double>();

  complex<double> gradient[2];

  for (unsigned ih=0; ih < max_harmonic; ih++) {

    complex<double> delexp_delvarphi (0.0, -2.0 * M_PI * double(ih+1));
    complex<double> Stokes_I (amps[ih]);

    // partial derivative with respect to phase
    gradient[0] = Stokes_I * delexp_delvarphi;
    // partial derivative with respect to gain
    gradient[1] = Stokes_I;

    for (unsigned ir=0; ir < 2; ir++)
      for (unsigned is=0; is < 2; is ++)
	curvature[ir][is] += (conj(gradient[ir]) * gradient[is]).real();
    
  }

  curvature /= mean_variance;

  covariance = inv(curvature);
}

void Pulsar::ScalarProfileFitAnalysis::delC_delS
(
 Matrix<2,2,double>& delC_delSre,
 Matrix<2,2,double>& delC_delSim,
 unsigned index
 ) const
{
  complex<double> delexp_delvarphi (0.0, -2.0 * M_PI * double(index+1));
  complex<double> Stokes_I (amps[index]);

  Matrix<2,2,double> delalpha_delSre;
  Matrix<2,2,double> delalpha_delSim;

  complex<double> delgradient_delS[2];
  complex<double> gradient[2];

  // partial derivative with respect to phase
  gradient[0] = Stokes_I * delexp_delvarphi;
  // partial derivative with respect to gain
  gradient[1] = Stokes_I;
  
  // partial derivative with respect to phase
  delgradient_delS[0] = delexp_delvarphi;
  // partial derivative with respect to gain
  delgradient_delS[1] = 1.0;
  
  for (unsigned ir=0; ir < 2; ir++) {
    
    for (unsigned is=0; is < 2; is ++) {
      
      delalpha_delSre[ir][is] = 
	( delgradient_delS[ir] * conj(gradient[is]) +
	  gradient[ir] * conj(delgradient_delS[is]) ).real();
      
      complex<double> i (0,1);
      
      delalpha_delSim[ir][is] = 
	( delgradient_delS[ir] * conj(gradient[is]) +
	  gradient[ir] * conj(delgradient_delS[is]) ).real();
      
    }

  }

  delalpha_delSre /= mean_variance;
  delalpha_delSim /= mean_variance;

  delC_delSre = -covariance*delalpha_delSre*covariance;
  delC_delSim = -covariance*delalpha_delSim*covariance;

}

Estimate<double> Pulsar::ScalarProfileFitAnalysis::get_error () const
{
  Matrix<2,2,double> curvature;
  const_cast<ScalarProfileFitAnalysis*>(this)->get_curvature (curvature);

  // the variance of the real and imaginary parts of the Stokes parameter
  double var_S = 0.5*mean_variance;

  // the variance of the phase shift variance
  double var_c_varphi = 0.0;

  for (unsigned ih=0; ih < max_harmonic; ih++) {

    Matrix<2,2,double> delC_delSre;
    Matrix<2,2,double> delC_delSim;

    delC_delS (delC_delSre, delC_delSim, ih);

    var_c_varphi += delC_delSre[0][0]*delC_delSre[0][0] * var_S;
    var_c_varphi += delC_delSim[0][0]*delC_delSim[0][0] * var_S;

  }

  return sqrt( Estimate<double> (covariance[0][0], var_c_varphi) );
}
