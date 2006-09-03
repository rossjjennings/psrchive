/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PolnProfileFitAnalysis.h"
#include "Pulsar/PolnProfile.h"

#include "Calibration/ReceptionModel.h"
#include "Calibration/TemplateUncertainty.h"
#include "MEAL/ProductRule.h"

#include "Pauli.h"
#include <assert.h>

using namespace std;

Pulsar::PolnProfileFitAnalysis::PolnProfileFitAnalysis ()
{
  compute_error = true;
  basis_insertion = 0;
  max_boost = 0.0;
  max_harmonic = 0;
  verbose = false;
}

//! When set, estimate the uncertainty in each attribute
void Pulsar::PolnProfileFitAnalysis::set_compute_error (bool flag)
{
  compute_error = flag;
}

void Pulsar::PolnProfileFitAnalysis::set_harmonic (unsigned index)
{
  double phase_shift = -2.0 * M_PI * double(index+1);

  fit->phase_axis.set_value (phase_shift);
  fit->model->set_input_index (index);
}

//! Given a coherency matrix, return the weighted conjugate matrix
Jones<double> weight (const Jones<double>& rho, const Stokes<double>& s)
{
  Stokes< complex<double> > stokes = complex_coherency( rho );

  for (unsigned ipol=0; ipol<4; ipol++)
    stokes[ipol] *= s[ipol];

  //cerr << "weight=" << s << endl;
  //cerr << "result=" << stokes << endl;

  return convert (stokes);
}

/*!
  Computes Equation 14 of van Straten (2006)
*/
void Pulsar::PolnProfileFitAnalysis::get_curvature (Matrix<8,8,double>& alpha)
{
  alpha = Matrix<8,8,double>();

  initialize();

  unsigned nharmonic = fit->model->get_num_input();
  if (max_harmonic && max_harmonic < nharmonic)
    nharmonic = max_harmonic;

  for (unsigned ih=0; ih < nharmonic; ih++) {

    set_harmonic (ih);
    model_result = fit->model->evaluate (&model_gradient);

    add_curvature (alpha);

  }
}

void Pulsar::PolnProfileFitAnalysis::add_curvature (Matrix<8,8,double>& alpha)
{
#ifdef _DEBUG
  for (unsigned ig=0; ig < model_gradient.size(); ig++)
    cerr << "g[" << fit->model->get_param_name(ig) << "]=\t"
	 << model_gradient[ig] << endl;
#endif

  assert( model_gradient.size() == 10 );

  for (unsigned ir=0; ir < 8; ir++) {
      
    Jones<double> delrho_deleta_r =
      fit->uncertainty->get_normalized (model_gradient[ir+2]);    
    
    for (unsigned is=0; is <= ir; is ++) {
      
      Jones<double> delrho_deleta_s = model_gradient[is+2];
      
      double one = 2*trace(delrho_deleta_r*herm(delrho_deleta_s)).real();
      
#if DOUBLE_CHECK_TWO_DIFFERENT_DESCRIPTIONS
      Stokes< complex<double> > dr = complex_coherency(model_gradient[ir+2]);
      Stokes< complex<double> > ds = complex_coherency(model_gradient[is+2]);
      
      double two = 0.0;
      for (unsigned i=0; i < 4; i++)
	two += (dr[i]*conj(ds[i])).real() * fit->uncertainty->get_inv_var(i);
      
      cerr << "one=" << one << " two=" << two << endl;
#endif
      
      alpha[ir][is] += one;
      if (ir != is)
        alpha[is][ir] += one;
    }
    
  }
  
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
    fprintf (stderr, "%12s", "unknown");
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



Jones<double> 
Pulsar::PolnProfileFitAnalysis::del_deleta (unsigned i,
					    const Jones<double>& K) const
{
  if (i == 0)
    return xform_result * K * herm(xform_result) * phase_gradient[2];

  i --;

  return ( xform_gradient[i] * K * herm(xform_result) +
	   xform_result * K * herm(xform_gradient[i]) ) * phase_result;
}

Jones<double> Pulsar::PolnProfileFitAnalysis::delrho_delS (unsigned k) const
{
  Stokes<double> q;
  q[k] = 1.0;
  return convert(q);
}

void Pulsar::PolnProfileFitAnalysis::delC_delS
( Matrix<8,8,double>& delC_delSre,
  Matrix<8,8,double>& delC_delSim, unsigned k ) const
{
  Matrix<8,8,double> delalpha_delSre;
  Matrix<8,8,double> delalpha_delSim;

  // over all rows
  for (unsigned ir=0; ir < 8; ir++) {
	
    Jones<double> del2rho_deleta_r = del_deleta (ir, delrho_delS(k));
    Jones<double> delrho_deleta_r = model_gradient[ir+2];    

    // over all columns
    for (unsigned is=0; is <= ir; is ++) {
	  
      Jones<double> del2rho_deleta_s = del_deleta (is, delrho_delS(k));
      Jones<double> delrho_deleta_s = model_gradient[is+2];
	  
      delalpha_delSre[ir][is] = 2.0 *
	trace( del2rho_deleta_r * herm(delrho_deleta_s) +
	       delrho_deleta_r  * herm(del2rho_deleta_s) ).real();

      complex<double> i (0,1);

      delalpha_delSim[ir][is] = 2.0 *
	trace( i*del2rho_deleta_r * herm(delrho_deleta_s) +
	       delrho_deleta_r    * herm(i*del2rho_deleta_s) ).real();

      if (is != ir) {
	delalpha_delSre[is][ir] = delalpha_delSre[ir][is];
	delalpha_delSim[is][ir] = delalpha_delSim[ir][is];
      }

    }
    
  }

  delalpha_delSre *= fit->uncertainty->get_inv_var(k);
  delalpha_delSim *= fit->uncertainty->get_inv_var(k);

  delC_delSre = -covariance * delalpha_delSre * covariance;
  delC_delSim = -covariance * delalpha_delSim * covariance;
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

  return ( delC_varphiJ_delS * (inv_C_JJ * C_varphiJ)
	   - C_varphiJ * (inv_C_JJ * delC_JJ_delS * inv_C_JJ * C_varphiJ) 
	   + C_varphiJ * (inv_C_JJ * delC_varphiJ_delS)
	   - R2_varphiJ * delc_varphi_delS ) / c_varphi;
}

Jones<double>
Pulsar::PolnProfileFitAnalysis::delrho_delB (unsigned b) const
{
  Jones<double> rho = fit->model->get_input()->evaluate();

#if 0

  cerr << "rho=" << rho << endl;
  cerr << "mod=" << model_result << endl;
  cerr << "basis=" << basis_result << endl;
  cerr << "bgrad[" << b << "]=" << basis_gradient[b] << endl;

#endif

  return basis_result * rho * herm(basis_gradient[b])
    + basis_gradient[b] * rho * herm(basis_result);
}

Stokes<double> Pulsar::PolnProfileFitAnalysis::delnoise
(const Jones<double>& J, const Jones<double>& J_grad)
{
  error.set_transformation (J);
  error.set_transformation_gradient (J_grad);

  Stokes<double> var = error.get_variance ();
  Stokes<double> var_grad = error.get_variance_gradient ();

#if 0
  cerr << "delnoise xform=" << J << endl
       << "          grad=" << J_grad << endl
       << "delnoise var=" << var << endl
       << "        grad=" << var_grad << endl;
#endif

  // normalize by the variance
  for (unsigned ipol=0; ipol<4; ipol++)
    var_grad[ipol] /= var[ipol];

  return var_grad;
}

//! Return weighted partial derivatives of variances wrt basis parameter
Stokes<double> Pulsar::PolnProfileFitAnalysis::delnoise_delB (unsigned b)
{
  return delnoise (basis_result, basis_gradient[b]);

#if 0
  Stokes<double> var = error.get_variance ();
  Stokes<double> var_grad = error.get_variance_gradient ();

  double param = basis->get_param(b);

  double step = 1e-14 * param;
  Stokes<double> expect_diff = step * var_grad;

  basis->set_param (b, param - 0.5*step);

  vector< Jones<double> > Jgrad;
  Jones<double> J = basis->evaluate (&Jgrad);

  error.set_transformation (J);
  error.set_transformation_gradient (Jgrad[b]);

  Stokes<double> var1 = error.get_variance ();
  Stokes<double> var1_grad = error.get_variance_gradient ();

  basis->set_param (b, param + 0.5*step);
  J = basis->evaluate (&Jgrad);

  error.set_transformation (J);
  error.set_transformation_gradient (Jgrad[b]);

  Stokes<double> var2 = error.get_variance ();
  Stokes<double> var2_grad = error.get_variance_gradient ();

  basis->set_param (b, param);

  Stokes<double> diff = var2 - var1;
  Stokes<double> curv = var2_grad - var1_grad;

  static unsigned errors = 0;
  static unsigned tests = 0;

  for (unsigned i=0; i < 4; i++) {

    double expect = expect_diff[i] + 0.5 * curv[i] * step * step;

    tests ++;

    if ( fabs((diff[i] - expect)/diff[i]) > 1e-2 &&
	 fabs((diff[i] - expect)/curv[i]) > 1 ) {

      errors ++;

#if 1
      cerr << "val=" << var << " grad=" << var_grad << endl;
      cerr << "1: val=" << var1 << " grad=" << var1_grad << endl;
      cerr << "2: val=" << var2 << " grad=" << var2_grad << endl;
      
      cerr << "diff["<<i<<"]=" << diff[i] << " expected=" << expect << endl;
      cerr << "curv=" << curv << " errors=" << errors 
	   << " tests=" << tests << endl;
#endif
      
    }

  }

#endif

}

Matrix<8,8,double>
Pulsar::PolnProfileFitAnalysis::delalpha_delB (unsigned ib)
{
  Matrix<8,8,double> delalpha_delB;

#ifdef CORRECT_CURVATURE
  Jones<double> rho = fit->model->get_input()->evaluate();
  rho = fit->uncertainty->get_normalized (rho);
#endif
 
  Jones<double> delR_delB = delrho_delB(ib);

  // over all rows
  for (unsigned ir=0; ir < 8; ir++) {

    Stokes<complex<double> > dr=complex_coherency(model_gradient[ir+2]);
    Stokes<complex<double> > d2r=complex_coherency(del_deleta(ir,delR_delB));

    // over all columns up to and including the diagonal
    for (unsigned is=0; is <= ir; is ++) {
	  
      Stokes<complex<double> > ds=complex_coherency(model_gradient[is+2]);
      Stokes<complex<double> > d2s=complex_coherency(del_deleta(is,delR_delB));

      double r2 = 0.0;

      for (unsigned i=0; i < 4; i++) {

	double iv = fit->uncertainty->get_inv_var(i);
	double dv = delN_delB[ib][i];

	r2 += (d2r[i]*conj(ds[i]) + dr[i]*conj(d2s[i])).real() * iv
	  - (dr[i]*conj(ds[i])).real() * iv * dv;

      }

      delalpha_delB[ir][is] = r2;

      if (ir != is)
	delalpha_delB[is][ir] = delalpha_delB[ir][is];

    }
    
  }

  // cerr << "del[" << ib << "]=" << delalpha_delB << endl;

  return delalpha_delB;
}

void Pulsar::PolnProfileFitAnalysis::initialize ()
{
  if (basis) {
    basis_result = basis->evaluate (&basis_gradient);
    basis_insertion->set_value( basis_result );

    delN_delB.resize (basis_gradient.size());
    for (unsigned i=0; i<delN_delB.size(); i++) {
      //cerr << "delnoise_delB " << i << endl;
      delN_delB[i] = delnoise_delB (i);
    }
  }
  else
    basis_result = 1;

  xform_result = xform->evaluate (&xform_gradient);

  // cerr << "xform=" << xform_result << endl;

  delN_delJ.resize (xform_gradient.size());
  for (unsigned i=0; i<delN_delJ.size(); i++) {
    delN_delJ[i] = delnoise (xform_result, xform_gradient[i]);
    //cerr << "delN_delJ[" << i << "]=" << delN_delJ[i] << endl;
  }
}

// compute the partial derivatives of the curvature matrix wrt basis parameters
void Pulsar::PolnProfileFitAnalysis::get_delalpha_delB
(vector< Matrix<8,8,double> >& delalpha_delbasis)
{
  delalpha_delbasis.resize (basis->get_nparam());
  for (unsigned ib=0; ib < basis->get_nparam(); ib++)
    delalpha_delbasis[ib] = Matrix<8,8,double>();

  initialize ();

  unsigned nharmonic = fit->model->get_num_input();
  if (max_harmonic && max_harmonic < nharmonic)
    nharmonic = max_harmonic;

  for (unsigned ih=0; ih < nharmonic; ih++) {

    unsigned bih = nharmonic - ih - 1;
    set_harmonic (bih);

    model_result = fit->model->evaluate (&model_gradient);
    phase_result = fit->phase_xform->evaluate (&phase_gradient);

    // over all basis parameters
    add_delalpha_delB (delalpha_delbasis);

  }


#if 0
  for (unsigned ib=0; ib < basis->get_nparam(); ib++)
    cerr << "del[" << ib << "]=" << endl << delalpha_delbasis[ib] << endl;
#endif

}

void Pulsar::PolnProfileFitAnalysis::add_delalpha_delB
(vector< Matrix<8,8,double> >& delalpha_delbasis)
{
  assert(delalpha_delbasis.size() == basis->get_nparam());

  // over all basis parameters
  for (unsigned ib=0; ib < basis->get_nparam(); ib++)
    delalpha_delbasis[ib] += delalpha_delB (ib);
}

//! Set the PolnProfileFit algorithm to be analysed
void Pulsar::PolnProfileFitAnalysis::set_fit (PolnProfileFit* f)
{
  fit = f;

  if (!fit)
    return;
    
  error.set_variance (f->standard_variance);
  xform = fit->get_transformation();

  if (basis)
    insert_basis ();

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

  if (max_harmonic && max_harmonic < fit->model->get_num_input())
    scalar.set_max_harmonic( max_harmonic );

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
  var_c_varphi = 0.0;

  unsigned nharmonic = fit->model->get_num_input();
  if (max_harmonic && max_harmonic < nharmonic)
    nharmonic = max_harmonic;

  for (unsigned ih=0; ih < nharmonic; ih++) {

    set_harmonic (ih);

    model_result = fit->model->evaluate (&model_gradient);
    phase_result = fit->phase_xform->evaluate (&phase_gradient);

    // over all four stokes parameters
    for (unsigned ip=0; ip < 4; ip++) {

      // calculate the partial derivative of the covariance matrix wrt S_ip
      Matrix<8,8,double> delC_delSre;
      Matrix<8,8,double> delC_delSim;
      delC_delS (delC_delSre, delC_delSim, ip);

      // the variance of the real and imaginary parts of the Stokes parameters
      double var_S = 0.5*fit->standard_variance[ip];

      var_c_varphi += ( delC_delSre[0][0]*delC_delSre[0][0] +
			delC_delSim[0][0]*delC_delSim[0][0] ) * var_S;

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

//! Get the multiple correlation and its gradient with respect to basis
double Pulsar::PolnProfileFitAnalysis::get_Rmult (std::vector<double>& grad)
{
  for (unsigned i=0; i<basis->get_nparam(); i++)
    cerr << "basis[" << i << "]=" << basis->get_param(i) << endl;

  basis_insertion->set_value( basis->evaluate() );

  // calculate the partial derivatives of the curvature wrt basis
  vector< Matrix<8,8,double> > delalpha_delbasis;
  get_delalpha_delB (delalpha_delbasis);

  // calculate the curvature matrix
  Matrix<8,8,double> curvature;
  get_curvature (curvature);

  // calculate the covariance matrix
  covariance = inv(curvature);

  // partition the covariance matrix
  conformal_partition (covariance, c_varphi, C_varphiJ, C_JJ);

  // calculate the inverse of the Jones parameter covariance matrix
  inv_C_JJ = inv(C_JJ);

  // the multiple correlation squared
  R2_varphiJ = C_varphiJ * (inv_C_JJ * C_varphiJ) / c_varphi;

  cerr << "c_varphi=" << c_varphi << " Rmult=" << R2_varphiJ << endl;

  grad.resize( basis->get_nparam() );

  for (unsigned ib=0; ib < basis->get_nparam(); ib++) {

    // calculate the partial derivative of the covariance matrix wrt basis
    Matrix<8,8,double> delC_delB;
    delC_delB = -covariance * delalpha_delbasis[ib] * covariance;

    // partition the partial derivative of the covariance matrix
    double delc_varphi_delB;
    Matrix <7,7,double> delC_JJ_delB;
    Vector <7,double> delC_varphiJ_delB;

    conformal_partition (delC_delB, delc_varphi_delB,
			 delC_varphiJ_delB, delC_JJ_delB);

    grad[ib] = 
      ( delC_varphiJ_delB * (inv_C_JJ * C_varphiJ)
	- C_varphiJ * ((inv_C_JJ*delC_JJ_delB*inv_C_JJ) * C_varphiJ)
	+ C_varphiJ * (inv_C_JJ * delC_varphiJ_delB)
	- C_varphiJ * (inv_C_JJ * C_varphiJ) / c_varphi * delc_varphi_delB )
      / c_varphi;

    cerr << "delRmult_delB[" << ib << "]= " << grad[ib] << endl;
      
  }

  cerr << "Rmult = " << R2_varphiJ << endl;

  return R2_varphiJ;
}

double Pulsar::PolnProfileFitAnalysis::get_c_varphi_error () const
{
  return sqrt(var_c_varphi);
}

void Pulsar::PolnProfileFitAnalysis::var_c_varphi_add ()
{
  // over all four stokes parameters
  for (unsigned ip=0; ip < 4; ip++) {

    // calculate the partial derivative of the covariance matrix wrt S_ip
    Matrix<8,8,double> delC_delSre;
    Matrix<8,8,double> delC_delSim;
    delC_delS (delC_delSre, delC_delSim, ip);

    // the variance of the real and imaginary parts of the Stokes parameters
    double var_S = 0.5*fit->standard_variance[ip];
    
    var_c_varphi += ( delC_delSre[0][0]*delC_delSre[0][0] + 
		      delC_delSim[0][0]*delC_delSim[0][0] ) * var_S;
  }
}

//! Get the variance of varphi and its gradient with respect to basis
double 
Pulsar::PolnProfileFitAnalysis::get_c_varphi (std::vector<double>* grad)
{
  //if (verbose)
  cerr << "boost:";
  for (unsigned i=1; i<4; i++)
    cerr << " " << basis->get_param(i);
  cerr << endl;

  // calculate the curvature matrix
  Matrix<8,8,double> curvature;
  get_curvature (curvature);

  // calculate the covariance matrix
  covariance = inv(curvature);

  double c_varphi = covariance[0][0];

  if (!grad)
    return c_varphi;

  // calculate the partial derivatives of the curvature wrt basis
  vector< Matrix<8,8,double> > delalpha_delbasis;
  get_delalpha_delB (delalpha_delbasis);

  grad->resize( basis->get_nparam() );

  double size = 0;

  for (unsigned ib=0; ib < basis->get_nparam(); ib++) {

    // calculate the partial derivative of the covariance matrix wrt basis
    Matrix<8,8,double> delC_delB;
    delC_delB = -covariance * delalpha_delbasis[ib] * covariance;

    (*grad)[ib] = delC_delB[0][0];
    
    if (verbose)
      cerr << "delvar_delB[" << ib << "]= " << (*grad)[ib] << endl;

    if (ib)
      size += (*grad)[ib] * (*grad)[ib];
  }

  cerr << "c_varphi = " << c_varphi << " grad=" << sqrt(size) 
       << " grad[0]=" << (*grad)[0] << endl;

  return c_varphi;
}



void Pulsar::PolnProfileFitAnalysis::compute_weights (unsigned nharmonic) 
{

  ScalarProfileFitAnalysis scalar;
  scalar.set_fit (fit);

  weights.resize( nharmonic );
  store_covariance.resize( nharmonic );

  for (unsigned i=0; i<nharmonic; i++) try {

    scalar.set_max_harmonic( i + 1 );

    Matrix<2,2,double> I_curvature;
    scalar.get_curvature (I_curvature);

    Matrix<2,2,double> I_covariance = inv(I_curvature);

    weights[i] = 1.0 / I_covariance[0][0];

    if (I_covariance[0][0] <= 0)
      weights[i] = 0.0;

    cerr << "weights[" << i << "]=" << weights[i] << endl;
  }
  catch (Error& error) {
    weights[i] = 0;
  }
  

}

double Pulsar::PolnProfileFitAnalysis::get_expected_relative_error 
(std::vector<unsigned>& histogram)
{
  double M_tot = 0.0;
  double I_tot = 0.0;
  unsigned total = 0;

  unsigned npts = histogram.size() -1;
  if (npts > weights.size())
    npts = weights.size();

  for (unsigned i=0; i < npts; i++) {
    if (weights[i] == 0)
      continue;
    unsigned count = histogram[i+1];
    double I_var = 1.0/weights[i];
    I_tot += count * I_var;
    double M_var = store_covariance[i];
    M_tot += count * M_var;
    cerr << i+1 << " " << count << " " << sqrt(M_var/I_var) << endl;
  }

  return sqrt (M_tot/I_tot);
}

//! Get the variance of varphi and its gradient with respect to basis
double 
Pulsar::PolnProfileFitAnalysis::get_C_varphi (std::vector<double>* grad)
{
  //if (verbose)
  cerr << "boost:";
  for (unsigned i=1; i<4; i++)
    cerr << " " << basis->get_param(i);
  cerr << endl;

  unsigned nparam = basis->get_nparam();

  // calculate the curvature matrix
  Matrix<8,8,double> curvature;

  // the partial derivatives of the curvature matrix wrt basis parameters
  vector< Matrix<8,8,double> > delalpha_delbasis (nparam);

  initialize();

  unsigned nharmonic = fit->model->get_num_input();
  if (max_harmonic && max_harmonic < nharmonic)
    nharmonic = max_harmonic;

  if (weights.size() != nharmonic)
    compute_weights (nharmonic);

  double C_varphi = 0;

  if (grad)  {
    *grad = vector<double> (nparam, 0.0);
    assert(grad->size() == nparam);
  }

  unsigned min_harmonic = 1;

  for (unsigned ih=0; ih < nharmonic; ih++) {

    set_harmonic (ih);
    model_result = fit->model->evaluate (&model_gradient);
    phase_result = fit->phase_xform->evaluate (&phase_gradient);

    add_curvature (curvature);

    store_covariance[ih] = 0;

    if (ih < min_harmonic)
      continue;

    // calculate the covariance matrix
    covariance = inv(curvature);

    if ( covariance[0][0] <= 0 )  {
      cerr << "minimum harmonic = " << ih + 1 << endl;
      min_harmonic = ih + 1;
    }

    store_covariance[ih] = covariance[0][0];

    C_varphi += weights[ih] * covariance[0][0];

    if (!grad)
      continue;

    add_delalpha_delB (delalpha_delbasis);

    for (unsigned ib=0; ib < nparam; ib++) {

      // calculate the partial derivative of the covariance matrix wrt basis
      Matrix<8,8,double> delC_delB;
      delC_delB = -covariance * delalpha_delbasis[ib] * covariance;

      (*grad)[ib] += weights[ih] * delC_delB[0][0];

    }

  }

  if (C_varphi == 0)
    throw Error (InvalidState, "get_C", "Insufficient signal-to-noise ratio");

  if (!grad)
    return C_varphi;

  double size=0;
  for (unsigned ib=0; ib < nparam; ib++)
    size += (*grad)[ib] * (*grad)[ib];

  cerr << "c_varphi = " << C_varphi << " grad=" << sqrt(size) 
       << " grad[0]=" << (*grad)[0] << endl;

  return C_varphi;
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

//! Set the transformation to be used to find the optimal basis
void Pulsar::PolnProfileFitAnalysis::set_basis (MEAL::Complex2* _basis)
{
  basis = _basis;

  if (fit)
    insert_basis();
}

void Pulsar::PolnProfileFitAnalysis::insert_basis ()
{
  if (basis_insertion)
    return;

  if (!fit || !basis)
    return;

  basis_insertion = new MEAL::Complex2Value;
  basis_insertion -> set_value( basis->evaluate() );

  MEAL::ProductRule<MEAL::Complex2>* p = new MEAL::ProductRule<MEAL::Complex2>;
  *p *= xform;
  *p *= basis_insertion;

  fit->set_transformation ( p );
}

    //! Get the transformation into the optimal basis
MEAL::Complex2* Pulsar::PolnProfileFitAnalysis::get_basis ()
{
  return basis;
}

//! Use or don't use the optimal transformation
void Pulsar::PolnProfileFitAnalysis::use_basis (bool use)
{
  if (use)
    basis_insertion -> set_value( basis->evaluate() );
  else
    basis_insertion -> set_value( Jones<double>::identity() );
}


//! Given a coherency matrix, return the weighted conjugate matrix
Jones<double> inv_weight (const Jones<double>& rho, const Stokes<double>& s)
{
  Stokes< complex<double> > stokes = complex_coherency( rho );

  for (unsigned ipol=0; ipol<4; ipol++)
    stokes[ipol] /= s[ipol];

  //cerr << "weight=" << s << endl;
  //cerr << "result=" << stokes << endl;

  return convert (stokes);
}


double Pulsar::PolnProfileFitAnalysis::get_cond_var (vector<double>& grad)
{
  if (verbose)
    for (unsigned ir=0; ir < xform_gradient.size(); ir++)
      cerr << "basis[" << ir << "]=" 
	   << xform->get_param(ir) << endl;

  xform_result = xform->evaluate (&xform_gradient);

  error.set_transformation (xform_result);
  Stokes<double> variance = error.get_variance();

  double var = 0.0;
  grad = vector<double> (xform_gradient.size(), 0.0);

  for (unsigned ih=0; ih < fit->model->get_num_input(); ih++) {

    unsigned bih = fit->model->get_num_input() - ih -1;

    set_harmonic (bih);
    model_result = fit->model->evaluate (&model_gradient);

    Stokes< complex<double> > S = complex_coherency(model_result);

    double weight = bih + 1;

    double this_var = 0;
    for (unsigned ipol=0; ipol<4; ipol++)
      this_var += weight * norm(S[ipol]) / variance[ipol];

    var += this_var;

    // cerr << ih << " " << this_var << " " << var << endl;

    for (unsigned ir=0; ir < xform_gradient.size(); ir++) {

      error.set_transformation_gradient (xform_gradient[ir]);
      Stokes<double> variance_gradient = error.get_variance_gradient();

      Stokes< complex<double> > Sg = complex_coherency(model_gradient[ir+3]);

      // cerr << "Sg[" << ir << "]=" << Sg << endl;

      for (unsigned ipol=0; ipol<4; ipol++) {

	double n1 = 2 * (conj(S[ipol])*Sg[ipol]).real();
	double n2 = norm(S[ipol]) * variance_gradient[ipol] / variance[ipol];

#if 0
	cerr << ir << " " << ipol << ":\n"
	  "n1=" << n1 << "\n"
	  "n2=" << n2 << endl;
#endif

	grad[ir] += weight * (n1 - n2) / variance[ipol];

      }

    }

  }
  
  var = 1/var;

  static double scale = 0;

  if (scale == 0) {
    scale = 1.0 / pow (10.0, floor(log(var)/log(10.0)));
    cerr << "scale=" << scale << endl;
  }

  double size = 0;

  for (unsigned ir=0; ir < xform_gradient.size(); ir++) {
    grad[ir] = - scale * var * var * grad[ir];
    if (verbose)
      cerr << "grad[" << ir << "]=" << grad[ir] << endl;
    size += grad[ir] * grad[ir];
  }

  size = sqrt(size);

  var *= scale;

  // if (verbose)
    cerr << "cond_var=" << var << " grad=" << size 
	 << " grad[0]=" << grad[0] << endl;

  return var;

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

  variance = fit->standard_variance[0];
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
  variance = v;
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

  curvature /= variance;

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

  delalpha_delSre /= variance;
  delalpha_delSim /= variance;

  delC_delSre = -covariance*delalpha_delSre*covariance;
  delC_delSim = -covariance*delalpha_delSim*covariance;

}

Estimate<double> Pulsar::ScalarProfileFitAnalysis::get_error () const
{
  Matrix<2,2,double> curvature;
  const_cast<ScalarProfileFitAnalysis*>(this)->get_curvature (curvature);

  // the variance of the real and imaginary parts of the Stokes parameter
  double var_S = 0.5*variance;

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
