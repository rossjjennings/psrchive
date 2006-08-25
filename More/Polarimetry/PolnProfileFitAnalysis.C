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

using namespace std;

Pulsar::PolnProfileFitAnalysis::PolnProfileFitAnalysis ()
{
  compute_error = true;
  basis_insertion = 0;
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

  for (unsigned ih=0; ih < fit->model->get_num_input(); ih++) {

    set_harmonic (ih);
    model_result = fit->model->evaluate (&model_gradient);

#ifdef CORRECT_CURVATURE
    Jones<double> rho = fit->model->get_input()->evaluate();
    rho = fit->uncertainty->get_normalized (rho);
#endif

#ifdef _DEBUG
    for (unsigned ig=0; ig < model_gradient.size(); ig++)
      cerr << "g[" << fit->model->get_param_name(ig) << "]=\t"
	   << model_gradient[ig] << endl;
#endif

    for (unsigned ir=0; ir < 8; ir++) {

      Jones<double> delrho_deleta_r =
	fit->uncertainty->get_normalized (model_gradient[ir+2]);    

#ifdef CORRECT_CURVATURE
      if (ir > 0 && delN_delJ.size() != 0) {
	Jones<double> correction = weight( rho, delN_delJ[ir-1] );
	cerr <<
	  "curv["<<ir<<"]: delR=" << delrho_deleta_r << "\n"
	  "         corr=" << correction << "\n"
	  "         grad=" << model_gradient[ir+2] << "\n"
	  "         delN=" << delN_delJ[ir-1]
	     << endl;
	delrho_deleta_r -= correction;
      }
#endif

      for (unsigned is=0; is < 8; is ++) {

	Jones<double> delrho_deleta_s = model_gradient[is+2];

	alpha[ir][is]+=2*trace(delrho_deleta_r*herm(delrho_deleta_s)).real();

      }

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
Pulsar::PolnProfileFitAnalysis::del_deleta (unsigned i,
					    const Jones<double>& K) const
{
  if (i == 0)
    return xform_result * K * herm(xform_result) * phase_gradient[2];

  i --;

  return ( xform_gradient[i] * K * herm(xform_result) * phase_result +
	   xform_result * K * herm(xform_gradient[i]) * phase_result );
}

Jones<double> delrho_delS (unsigned k)
{
  Stokes<double> q;
  q[k] = 1.0;
  return convert (q);
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
    for (unsigned is=0; is < 8; is ++) {
	  
      Jones<double> del2rho_deleta_s = del_deleta (is, delrho_delS(k));
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

  delalpha_delSre *= fit->uncertainty->get_inv_var(k);
  delalpha_delSim *= fit->uncertainty->get_inv_var(k);

  delC_delSre = -covariance * delalpha_delSre * covariance;
  delC_delSim = -covariance * delalpha_delSim * covariance;
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
(const Jones<double>& xform, const Jones<double>& xform_grad)
{

  error.set_transformation (xform);
  error.set_transformation_gradient (xform_grad);

  Stokes<double> var = error.get_variance ();
  Stokes<double> var_grad = error.get_variance_gradient ();

#if 0
  cerr << "delnoise xform=" << xform << endl
       << "          grad=" << xform_grad << endl
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
  return delnoise (xform_result*basis_result, xform_result*basis_gradient[b]);
}

Matrix<8,8,double>
Pulsar::PolnProfileFitAnalysis::delalpha_delB (unsigned ib)
{
  Matrix<8,8,double> delalpha_delB;

#ifdef CORRECT_CURVATURE
  Jones<double> rho = fit->model->get_input()->evaluate();
  rho = fit->uncertainty->get_normalized (rho);
#endif

  // over all rows
  for (unsigned ir=0; ir < 8; ir++) {

    Jones<double> delrho_deleta_r
      = fit->uncertainty->get_normalized( model_gradient[ir+2] );

    Jones<double> del2rho_deleta_r
      = fit->uncertainty->get_normalized( del_deleta(ir, delR_delB[ib]) );

#if 0
    Jones<double> correction = weight( delrho_deleta_r, delN_delB[ib] );

    //cerr << "del2rho_deleta_r=" << delrho_deleta_r << endl;
    //cerr << "      correction=" << correction << endl;
    del2rho_deleta_r -= correction;
#endif

#ifdef CORRECT_CURVATURE
    if (ir > 0) {
      //cerr << "delrho_deleta_r=" << delrho_deleta_r << endl;
      //cerr << "correction=" << delN_delJ[ir-1] << endl;
      delrho_deleta_r -= weight( rho, delN_delJ[ir-1] );
    }
#endif

    // over all columns
    for (unsigned is=0; is < 8; is ++) {
	  
      Jones<double> del2rho_deleta_s = del_deleta(is, delR_delB[ib]);
      Jones<double> delrho_deleta_s = model_gradient[is+2];
	  
      delalpha_delB[ir][is] = 2.0 *
	trace( del2rho_deleta_r * herm(delrho_deleta_s) +
	       delrho_deleta_r  * herm(del2rho_deleta_s) ).real();

    }
    
  }

  return delalpha_delB;
}

void Pulsar::PolnProfileFitAnalysis::initialize ()
{
  if (basis) {
    basis_result = basis->evaluate (&basis_gradient);

    delN_delB.resize (basis_gradient.size());
    delR_delB.resize (basis_gradient.size());
    Jones<double> inv_basis = inv(basis_result);
    for (unsigned i=0; i<delN_delB.size(); i++) {
      //cerr << "delnoise_delB " << i << endl;
      delN_delB[i] = delnoise_delB (i);
      delR_delB[i] = inv_basis * delrho_delB(i) * herm(inv_basis);
    }
  }

  xform_result = fit->transformation->evaluate (&xform_gradient);
  delN_delJ.resize (xform_gradient.size());
  for (unsigned i=0; i<delN_delJ.size(); i++) {
    delN_delJ[i] = delnoise (xform_result, xform_gradient[i]);
    //cerr << "delN_delJ[" << i << "]=" << delN_delJ[i] << endl;
  }
}

// compute the partial derivatives of the curvature matrix wrt basis parameters
void Pulsar::PolnProfileFitAnalysis::delalpha_delB
(vector< Matrix<8,8,double> >& delalpha_delbasis)
{
  delalpha_delbasis.resize (basis->get_nparam());
  for (unsigned ib=0; ib < basis->get_nparam(); ib++)
    delalpha_delbasis[ib] = Matrix<8,8,double>();

  initialize ();

  for (unsigned ih=0; ih < fit->model->get_num_input(); ih++) {

    set_harmonic (ih);
    model_result = fit->model->evaluate (&model_gradient);
    phase_result = fit->phase_xform->evaluate (&phase_gradient);

    // over all basis parameters
    for (unsigned ib=0; ib < basis->get_nparam(); ib++)
      delalpha_delbasis[ib] += delalpha_delB (ib);

  }
}

//! Set the PolnProfileFit algorithm to be analysed
void Pulsar::PolnProfileFitAnalysis::set_fit (PolnProfileFit* f)
{
  fit = f;

  if (!fit)
    return;

  if (basis)
    for (unsigned i=0; i<basis->get_nparam(); i++)
      cerr << "basis[" << i << "]=" << basis->get_param(i) << endl;

  error.set_variance (f->standard_variance);

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
    phase_result = fit->phase_xform->evaluate (&phase_gradient);

    // over all four stokes parameters
    for (unsigned ip=0; ip < 4; ip++) {

      // calculate the partial derivative of the covariance matrix wrt S_ip
      Matrix<8,8,double> delC_delSre;
      Matrix<8,8,double> delC_delSim;
      delC_delS (delC_delSre, delC_delSim, ip);

      // the variance of the real and imaginary parts of the Stokes parameters
      double var_S = 0.5/fit->uncertainty->get_inv_var(ip);

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

//! Get the multiple correlation and its gradient with respect to basis
double Pulsar::PolnProfileFitAnalysis::get_Rmult (std::vector<double>& grad)
{
  for (unsigned i=0; i<basis->get_nparam(); i++)
    cerr << "basis[" << i << "]=" << basis->get_param(i) << endl;

  basis_insertion->set_value( basis->evaluate() );

  // calculate the partial derivatives of the curvature wrt basis
  vector< Matrix<8,8,double> > delalpha_delbasis;
  delalpha_delB (delalpha_delbasis);

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


//! Get the variance of varphi and its gradient with respect to basis
double 
Pulsar::PolnProfileFitAnalysis::get_var_varphi (std::vector<double>& grad)
{
  for (unsigned i=0; i<basis->get_nparam(); i++)
    cerr << "basis[" << i << "]=" << basis->get_param(i) << endl;

  basis_insertion->set_value( basis->evaluate() );

  // calculate the partial derivatives of the curvature wrt basis
  vector< Matrix<8,8,double> > delalpha_delbasis;
  delalpha_delB (delalpha_delbasis);

  // calculate the curvature matrix
  Matrix<8,8,double> curvature;
  get_curvature (curvature);

  // calculate the covariance matrix
  covariance = inv(curvature);

  cerr << "compute delalpha_delB" << endl;

  grad.resize( basis->get_nparam() );

  for (unsigned ib=0; ib < basis->get_nparam(); ib++) {

    // calculate the partial derivative of the covariance matrix wrt basis
    Matrix<8,8,double> delC_delB;
    delC_delB = -covariance * delalpha_delbasis[ib] * covariance;

    grad[ib] = delC_delB[0][0];
    cerr << "delvar_delB[" << ib << "]= " << delC_delB[0][0] << endl;
      
  }

  cerr << "c_varphi = " << covariance[0][0] << endl;

  return covariance[0][0];
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

  cerr << "Inserting basis transformation" << endl;

  basis_insertion = new MEAL::Complex2Value;
  basis_insertion -> set_value( basis->evaluate() );

  MEAL::Complex2* xform = fit->get_transformation();

  MEAL::ProductRule<MEAL::Complex2>* p = new MEAL::ProductRule<MEAL::Complex2>;
  *p *= xform;
  *p *= basis_insertion;

  fit->set_transformation ( p );
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
  for (unsigned ir=0; ir < xform_gradient.size(); ir++)
    cerr << "basis[" << ir << "]=" << fit->transformation->get_param(ir) << endl;

  xform_result = fit->transformation->evaluate (&xform_gradient);

  error.set_transformation (xform_result);
  Stokes<double> variance = error.get_variance();

  double var = 0.0;
  grad = vector<double> (xform_gradient.size(), 0.0);

  for (unsigned ih=0; ih < fit->model->get_num_input(); ih++) {

    set_harmonic (ih);
    model_result = fit->model->evaluate (&model_gradient);

    Stokes< complex<double> > S = complex_coherency(model_result);

    for (unsigned ipol=0; ipol<4; ipol++)
      var += norm(S[ipol])/variance[ipol];

    for (unsigned ir=0; ir < xform_gradient.size(); ir++) {

      error.set_transformation_gradient (xform_gradient[ir]);
      Stokes<double> variance_gradient = error.get_variance_gradient();

      Stokes< complex<double> > Sg = complex_coherency(model_gradient[ir+3]);

      for (unsigned ipol=0; ipol<4; ipol++) {

	double n1 = 2 * (conj(S[ipol])*Sg[ipol]).real();
	double n2 = norm(S[ipol]) * variance_gradient[ipol] / variance[ipol];

#if 0
	cerr << ir << " " << ipol << ":\n"
	  "n1=" << n1 << "\n"
	  "n2=" << n2 << endl;
#endif

	grad[ir] += (n1 - n2) / variance[ipol];

      }

    }

  }
  
  for (unsigned ir=0; ir < xform_gradient.size(); ir++)
    cerr << "grad[" << ir << "]=" << grad[ir] << endl;

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

  variance = 1.0/fit->uncertainty->get_inv_var(0);
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
