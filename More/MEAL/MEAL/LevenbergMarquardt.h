//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/LevenbergMarquardt.h,v $
   $Revision: 1.5 $
   $Date: 2004/11/23 11:25:43 $
   $Author: straten $ */

#ifndef __Levenberg_Marquardt_h
#define __Levenberg_Marquardt_h

#include "MEAL/GaussJordan.h"
#include "Estimate.h"
#include "Error.h"

#ifdef sun
#include <ieeefp.h>
#endif

namespace MEAL {

  //! Implements the Levenberg-Marquardt algorithm for non-linear least squares
  /*! This template class implements the nonlinear least squares
    fitting algorithm suggested by Levenberg, developed by Marquardt,
    and described in Numerical Recipes Chapter 15.5.  Use of the
    template methods of this class requires the definition of a model class
    that satisfies the following template:

    <pre>
    class Mt {

      //! Return the number of parameters in the model
      unsigned get_nparam () const;

      //! Return the value of the specified parameter
      double get_param (unsigned iparam) const;

      //! Set the value of the specified parameter
      void set_param (unsigned iparam, ) const;

      //! Return true if parameter at index is to be fitted
      bool get_infit (unsigned index) const;

      //! Return the value and gradient (wrt model parameters) of model at x
      Yt evaluate (std::vector<Gt>* gradient);

    };
    </pre>

    The type of the gradient, Gt, is explicity specified in the
    declaration of this template class.  The types of Xt and Yt are
    implicitly specified by the template instantiation of the methods
    of this class.  If Yt or Gt is not of type float or double, there
    must also be defined:

    <pre>
    //! a subtraction operator:
    const Yt operator - (const Yt &, const Yt &);

    //! a multiplication operator:
    const Gt operator * (const Yt &, const Gt &);

    <\pre>

    The LevenbergMarquardt class is used in three stages:
    <UL>
    <LI> call to ::init() with data and model
    <LI> repeated calls to ::iter() with data and model, comparing the chisq
    returned in order to determine convergence (or not) of fit
    <LI>call to ::result() to get curvature and covariance matrices
    </UL>
  */
  template <class Grad>
  class LevenbergMarquardt {
    
  public:
    static unsigned verbose;
    
    LevenbergMarquardt () { 
      lamda_increase_factor = 10.0;
      lamda_decrease_factor = 0.1;
      singular_threshold = 1e-8;
    }
    
    //! returns initial chi-squared
    template <class Xt, class Yt, class Et, class Mt>
    float init (const std::vector< Xt >& x,
		const std::vector< Estimate<Yt,Et> >& y,
		Mt& model);
    
    //! returns next chi-squared (better or worse)
    template <class Xt, class Yt, class Et, class Mt>
    float iter (const std::vector< Xt >& x,
		const std::vector< Estimate<Yt,Et> >& y,
		Mt& model);

    //! returns the best-fit answers
    template <class Mt>
    void result (Mt& model,
		 std::vector<std::vector<double> >& covariance = null_arg,
		 std::vector<std::vector<double> >& curvature = null_arg);

    //! lamda determines the dominance of the steepest descent method
    float lamda;

    float lamda_increase_factor;
    float lamda_decrease_factor;

    //! Singular Matrix threshold
    /*! Passed to Numerical::GaussJordan, this attribute is used to
      decide when the curvature matrix is close to singular. */
    float singular_threshold;

  protected:

    //! Inverts H*d=b, where: H=modified Hessian, d=delta, b=gradient
    template <class Mt> void solve_delta (const Mt& model);
    
    //! returns chi-squared and calculates the Hessian matrix and gradient
    template <class Xt, class Yt, class Et, class Mt>
    float calculate_chisq (const std::vector< Xt >& x,
			   const std::vector< Estimate<Yt,Et> >& y,
			   Mt& model);

  private:

    //! gradient of model: partial derivatives wrt its parameters
    std::vector<Grad> gradient;

    //! gradient of chi-squared wrt model parameters
    std::vector<double> beta;

    //! curvature matrix
    std::vector<std::vector<double> > alpha;

    //! next change to model
    std::vector<std::vector<double> > delta;

    //! chi-squared of best fit
    float best_chisq;                     

    //! curvature matrix (one half of Hessian matrix) of best fit
    std::vector<std::vector<double> > best_alpha;
    //! gradient of chi-squared of best fit
    std::vector<double> best_beta;

    //! The parameters of the current model
    std::vector<double> backup;

    static std::vector<std::vector<double> > null_arg;

  };

  //! Enables broader use of the LevenberMarquardt algorithm
  template<class Et>
  class WeightingScheme {

  public:

    //! Default constructor
    WeightingScheme (Et variance = 1.0)
    {
      set_variance (variance);
    }

    //! Set the variance
    void set_variance (const Et& variance)
    {
      inverse_variance = 1.0 / variance;
    }

    //! Return the norm of the value
    template<class Type>
    inline Type norm (const Type& x) const
    {
      return x*x; 
    }

    template<class Type>
    Type get_weighted_conjugate (const Type& data) const
    {
      return data * inverse_variance;
    }

    template<class Type>
    Type get_weighted_norm (const Type& data) const
    { 
      return norm(data) * inverse_variance;
    }

    Et inverse_variance;

  };

  //! Calculates alpha and beta
  template <class Mt, class Xt, class Yt, class Et, class Grad>
  float lmcoff (// input
		Mt& model,
		const Xt& abscissa,
		const Estimate<Yt,Et>& data,
		// storage
		std::vector<Grad>& gradient,
		// output
		std::vector<std::vector<double> >& alpha,
		std::vector<double>& beta);
  
  //! Calculates alpha and beta
  /*! Wt must be a weighting scheme */
  template <class Mt, class Yt, class Wt, class Grad>
  float lmcoff1 (// input
		 Mt& model,
		 const Yt& delta_data,
		 const Wt& weighting_scheme,
		 const std::vector<Grad>& gradient,
		 // output
		 std::vector<std::vector<double> >& alpha,
		 std::vector<double>& beta);
  
}

template <class Grad>
std::vector<std::vector<double> > MEAL::LevenbergMarquardt<Grad>::null_arg;

template <class Grad>
unsigned MEAL::LevenbergMarquardt<Grad>::verbose = 0;

template <class Grad>
template <class Xt, class Yt, class Et, class Mt>
float MEAL::LevenbergMarquardt<Grad>::init
(const std::vector< Xt >& x,
 const std::vector< Estimate<Yt,Et> >& y,
 Mt& model)
{
  if (verbose > 2)
    cerr << "MEAL::LevenbergMarquardt<Grad>::init" << endl;

  // size all of the working space arrays
  alpha.resize  (model.get_nparam());
  beta.resize   (model.get_nparam());
  delta.resize  (model.get_nparam());
  backup.resize (model.get_nparam());
  for (unsigned j=0; j<model.get_nparam(); j++) {
    alpha[j].resize (model.get_nparam());
    delta[j].resize (1);
  }

  best_chisq = calculate_chisq (x, y, model);
  best_alpha = alpha;
  best_beta = beta;
  lamda = 0.001;

  if (verbose > 0)
    cerr << "MEAL::LevenbergMarquardt<Grad>::init chisq=" 
	 << best_chisq << endl;

  return best_chisq;
}

template<class T>
void verify_orthogonal (const std::vector<std::vector<double > >& alpha, const T& model)
{
  unsigned nrow = alpha.size();

  if (!nrow)
    return;

  std::vector<double> row_mod (nrow, 0.0);

  // calculate the size of each row vector
  for (unsigned irow=0; irow<nrow; irow++) {
    for (unsigned jcol=0; jcol<nrow; jcol++) 
      row_mod[irow] += alpha[irow][jcol] * alpha[irow][jcol];
    row_mod[irow] = sqrt(row_mod[irow]);
  }

  unsigned kparam = 0;

  for (unsigned krow=0; krow<nrow; krow++) {

    while (!model.get_infit(kparam))
      kparam ++;

    unsigned iparam = 0;

    for (unsigned irow=krow+1; irow<nrow; irow++) {

      while (!model.get_infit(iparam))
	iparam ++;

      if (row_mod[krow] == 0 || row_mod[irow] == 0)
        continue;
      
      double covar = 0.0;
      for (unsigned jcol=0; jcol<nrow; jcol++)
        covar += alpha[krow][jcol] * alpha[irow][jcol];
      covar /= row_mod[krow] * row_mod[irow];

      if (!finite(covar)) {
        cerr << "NaN or Inf in covariance matrix" << endl;
        return;
      }

      if ( fabs( 1.0-fabs(covar) ) < 1e-3 )
        cerr << model.get_param_name(kparam) << " (row " << krow << ") and "
	     << model.get_param_name(iparam) << " (row " << irow << ")"
	     " covar=" << covar << endl;
    }

  }

}

// /////////////////////////////////////////////////////////////////////////
// MEAL::LevenbergMarquardt<Grad>::solve_delta
// /////////////////////////////////////////////////////////////////////////

/*! Using the curvature matrix and gradient stored in the best_alpha
  and best_beta attributes and the current value of lamda, solve
  Equation 15.5.14 for the change in model parameters, delta.

  \retval delta attribute

  This method also uses the alpha attribute to temporarily hold alpha'
*/
template <class Grad>
template <class Mt>
void MEAL::LevenbergMarquardt<Grad>::solve_delta (const Mt& model)
{
  if (verbose > 2)
    cerr << "MEAL::LevenbergMarquardt<Grad>::solve_delta" << endl;

  if (alpha.size() != model.get_nparam())
    throw Error (InvalidState, 
		 "MEAL::LevenbergMarquardt<Grad>::solve_delta",
		  "alpha.size=%d != model.nparam=%d", 
		 alpha.size(), model.get_nparam());

#ifndef _DEBUG
  if (verbose > 2)
#endif
    cerr << "MEAL::LevenbergMarquardt<Grad>::solve_delta lamda="
	 << lamda << " nparam=" << model.get_nparam() << endl;

  unsigned iinfit = 0;
  for (unsigned ifit=0; ifit<model.get_nparam(); ifit++)
    if (model.get_infit(ifit)) {

      unsigned jinfit = 0;
      for (unsigned jfit=0; jfit<model.get_nparam(); jfit++)
	if (model.get_infit(jfit)) {
	  alpha[iinfit][jinfit]=best_alpha[ifit][jfit];
	  jinfit ++;
	}

      alpha[iinfit][iinfit] *= (1.0 + lamda);
      delta[iinfit][0]=best_beta[ifit];
      iinfit ++;

    }


  if (iinfit == 0)
    throw Error (InvalidState,
		 "MEAL::LevenbergMarquardt<Grad>::solve_delta"
		  "no parameters in fit");

  if (verbose > 2)
    cerr << "MEAL::LevenbergMarquardt<Grad>::solve_delta for " << iinfit
	 << " parameters" << endl;

  //! curvature matrix
  std::vector<std::vector<double> > temp_copy (alpha);

  try {
    // invert Equation 15.5.14
    MEAL::GaussJordan (alpha, delta, iinfit, singular_threshold);
  }
  catch (Error& error)  {
    cerr << "Numerical::GaussJordan failed" << endl;
    verify_orthogonal (temp_copy, model);
    throw error += "MEAL::LevenbergMarquardt<Grad>::solve_delta";
  }

  if (verbose > 2)
    cerr << "MEAL::LevenbergMarquardt<Grad>::solve_delta exit" << endl;
}

// /////////////////////////////////////////////////////////////////////////
// MEAL::LevenbergMarquardt<Grad>::iter
// /////////////////////////////////////////////////////////////////////////

/*! This method calls solve_delta to determine the change in model
  parameters based on the current value of lamda, the curvature matrix
  and gradient, as stored in the best_alpha and best_beta attributes.

  The model is updataed using the delta attribute, and the new
  chi-squared, curvature and gradient are calculated.  If the new
  chi-squared is better than best_chisq, then the model is kept, the
  best_alpha, best_beta, and best_chisq attributes are updated, and
  lamda is decreased for the next trial.  If the new chi-squared is
  worse, the model is restored to its previous state, and lamda is
  increased for the next trial.

  \return chi-squared of model
  \retval model best model 

  This method also uses the beta attribute to unpack delta.
*/
template <class Grad>
template <class Xt, class Yt, class Et, class Mt>
float MEAL::LevenbergMarquardt<Grad>::iter
(const std::vector< Xt >& x,
 const std::vector< Estimate<Yt,Et> >& y,
 Mt& model)
{
  if (verbose > 2)
    cerr << "MEAL::LevenbergMarquardt<Grad>::iter" << endl;

  solve_delta (model);

  // After call to solve_delta, delta contains required change in model
  // parameters.  Update the model.

  if (verbose > 2)
    cerr << "MEAL::LevenbergMarquardt<Grad>::iter update model" << endl;

  unsigned iinfit = 0;
  for (unsigned ifit=0; ifit<model.get_nparam(); ifit++) {

    double change = 0.0;

    if (model.get_infit(ifit)) {
      change = delta[iinfit][0];
      iinfit ++;
    }

    backup[ifit] = model.get_param (ifit);

    if (verbose > 2)
      cerr << "   delta[" << ifit << "]=" << change << endl;

    model.set_param (ifit, backup[ifit] + change);
  }

  if (verbose > 2)
    cerr << "MEAL::LevenbergMarquardt<Grad>::iter"
      " calculate new chisq" << endl;
  float new_chisq = calculate_chisq (x, y, model);

  if (new_chisq < best_chisq) {

    lamda *= lamda_decrease_factor;

    if (verbose)
      cerr << "MEAL::LevenbergMarquardt<Grad>::iter new chisq="
           << new_chisq << "\n  better fit; lamda=" << lamda << endl;

    best_chisq = new_chisq;
    best_alpha = alpha;
    best_beta  = beta;

  }
  else {

    lamda *= lamda_increase_factor;

    if (verbose)
      cerr << "MEAL::LevenbergMarquardt<Grad>::iter new chisq="
           << new_chisq << "\n  worse fit; lamda=" << lamda << endl;

    // restore the old model
    for (unsigned iparm=0; iparm<model.get_nparam(); iparm++)
      model.set_param (iparm, backup[iparm]);

  }

  return new_chisq;
}

// /////////////////////////////////////////////////////////////////////////
// MEAL::LevenbergMarquardt<Grad>::result
// /////////////////////////////////////////////////////////////////////////

/* After a call to init or iter, best_alpha contains the last
   curvature matrix computed.  A call is made to solve_delta with
   lamda=0; member "alpha" will then contain the covariance matrix.

   \retval covar the covariance matrix
   \retval curve the curvature matrix
*/
template <class Grad>
template <class Mt>
void 
MEAL::LevenbergMarquardt<Grad>::result (Mt& model,
					     std::vector<std::vector<double> >& covar,
					     std::vector<std::vector<double> >& curve)
{
  if (verbose > 2)
    cerr << "MEAL::LevenbergMarquardt<Grad>::result" << endl;

  if (&curve != &null_arg)
    curve = best_alpha;

  lamda = 0.0;
  solve_delta (model);

  if (&covar == &null_arg)
    return;

  covar.resize (model.get_nparam());

  unsigned iindim = 0;
  for (unsigned idim=0; idim < model.get_nparam(); idim++) {
    covar[idim].resize (model.get_nparam());
 
    if (!model.get_infit(idim))
      for (unsigned jdim=0; jdim < model.get_nparam(); jdim++)
	covar[idim][jdim] = 0;
    else {
      unsigned jindim = 0;
      for (unsigned jdim=0; jdim < model.get_nparam(); jdim++)
	if (model.get_infit(jdim)) {
	  covar[idim][jdim] = alpha [iindim][jindim];
	  jindim ++;
	}
	else
	  covar[idim][jdim] = 0;

      iindim ++;
    }
  }  
}

// /////////////////////////////////////////////////////////////////////////
// MEAL::LevenbergMarquardt<Grad>::chisq
// /////////////////////////////////////////////////////////////////////////

/* Given a set of abscissa, ordinate, ordinate error, and a model,
   compute the: - normalized squared difference "chi-squared" -
   chi-squared gradient "beta" - Hessian matrix "alpha"

   \return chi-squared

   \retval alpha attribute
   \retval beta attribute

   This method uses the gradient attribute to store the model gradient
*/ 
template <class Grad>
template <class Xt, class Yt, class Et, class Mt>
float MEAL::LevenbergMarquardt<Grad>::calculate_chisq
(const std::vector< Xt >& x,
 const std::vector< Estimate<Yt,Et> >& y,
 Mt& model)
{
  if (verbose > 2)
    cerr << "MEAL::LevenbergMarquardt<Grad>::chisq" << endl;

  if (alpha.size() != model.get_nparam())
    throw Error (InvalidState, "MEAL::LevenbergMarquardt<Grad>::chisq",
		 "alpha.size=%d != model.nparam=%d",
		 alpha.size(), model.get_nparam());

  if (y.size() < x.size())
    throw Error (InvalidParam, "MEAL::LevenbergMarquardt<Grad>::chisq",
		 "y.size=%d < x.size=%d", y.size(), x.size());

  // initialize sums
  double Chisq = 0.0;
  for (unsigned j=0; j<alpha.size(); j++) {
    for (unsigned k=0; k<=j; k++)
      alpha[j][k] = 0.0;
    beta[j] = 0.0;
  }

  for (unsigned ipt=0; ipt < x.size(); ipt++) {

    if (verbose > 2)
      cerr << "MEAL::LevenbergMarquardt<Grad>::chisq lmcoff[" << ipt
	   << "/" << x.size() << "]" << endl;

    Chisq += lmcoff (model, x[ipt], y[ipt],
		     gradient, alpha, beta);
  }

  // populate the symmetric half of the curvature matrix
  for (unsigned ifit=1; ifit<model.get_nparam(); ifit++)
    for (unsigned jfit=0; jfit<ifit; jfit++)
      alpha[jfit][ifit]=alpha[ifit][jfit];

  return Chisq;
}

template <class Mt, class Xt, class Yt, class Et, class Grad>
float MEAL::lmcoff (// input
			   Mt& model,
			   const Xt& abscissa,
			   const Estimate<Yt,Et>& data,
			   // storage
			   std::vector<Grad>& gradient,
			   // output
			   std::vector<std::vector<double> >& alpha,
			   std::vector<double>& beta)
{
  if (LevenbergMarquardt<Grad>::verbose > 2)
    cerr << "MEAL::lmcoff data val=" << data.val
	 << " var=" << data.var << endl;
 
  abscissa.apply();

  WeightingScheme<Et> weight (data.var);
  Yt model_y = model.evaluate (&gradient);
  Yt delta_y = data.val - model_y;

  return lmcoff1 (model, delta_y, weight, gradient, alpha, beta);
}


template <class Mt, class Yt, class Wt, class Grad>
float MEAL::lmcoff1 (// input
			  Mt& model,
			  const Yt& delta_y,
			  const Wt& weight,
			  const std::vector<Grad>& gradient,
			  // output
			  std::vector<std::vector<double> >& alpha,
			  std::vector<double>& beta)
{
  //! The traits of the gradient element
  ElementTraits<Grad> traits;

  if (LevenbergMarquardt<Grad>::verbose > 2)
    cerr << "MEAL::lmcoff1 delta_y=" << delta_y << endl;

  Yt w_delta_y = weight.get_weighted_conjugate (delta_y);

  for (unsigned ifit=0; ifit < model.get_nparam(); ifit++) {
    
    if (model.get_infit(ifit)) {
      
      // Equation 15.5.6 (with 15.5.8)
      beta[ifit] += traits.to_real (w_delta_y * gradient[ifit]);

      Grad w_gradient = weight.get_weighted_conjugate (gradient[ifit]);

      // Equation 15.5.11
      for (unsigned jfit=0; jfit <= ifit; jfit++)
	if (model.get_infit(jfit))
	  alpha[ifit][jfit] += traits.to_real (w_gradient * gradient[jfit]);
    }
  }

  // Equation 15.5.5
  float chisq = weight.get_weighted_norm (delta_y);

  if (LevenbergMarquardt<Grad>::verbose > 2)
    cerr << "MEAL::lmcoff1 chisq=" << chisq << endl;

  return chisq;
}



#endif
