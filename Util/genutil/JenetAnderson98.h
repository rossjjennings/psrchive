//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/JenetAnderson98.h,v $
   $Revision: 1.6 $
   $Date: 2006/04/19 15:13:08 $
   $Author: straten $ */

#ifndef __Jenet_Anderson_98
#define __Jenet_Anderson_98

#include <vector>

//! Statistics of two-bit quantization and artifact correction parameters
/*!  Given either the fraction of digitized samples in the low voltage
  state, \f$\Phi\f$, or the instantaneous power normalized by the mean
  power, \f$\sigma_n=\sigma/\langle\sigma\rangle\f$, this class
  computes
  <ol>
  <li> the output levels that minimize signal distortion 
       (dynamic level setting), and
  <li> the fractional quantization noise (scattered power correction).

  \param Phi fraction of low voltage state samples, or
  \param sigma_n normalized power

  \retval lo the optimal low voltage output level
  \retval hi the optimal hi voltage output level
  \retval A the fractional quantization noise

  Furthermore, given the sampling threshold, \f$t\f$, this class
  computes the mean, variance, and probability distribution of
  \f$\Phi\f$.
*/
class JenetAnderson98 {

 public:

  static const double optimal_threshold;

  //! Default constructor
  JenetAnderson98 ();

  //! Set the fraction of samples in the low voltage state
  void set_Phi (double Phi);

  //! Set the normalized, undigitized power
  void set_sigma_n (double sigma_n);

  //! Get the optimal high voltage output levels, JA98 Eq.40
  double get_hi () const { return hi; }

  //! Get the optimal low voltage output levels, JA98 Eq.41
  double get_lo () const { return lo; }

  //! Get the slope of digitized vs undigitized correlation, JA98 Eq.43
  double get_A () const { return A; }

  //! Set the sampling threshold
  void set_threshold (double t = optimal_threshold);
  double get_threshold () const { return threshold; }

  //! Get the expectation value of Phi, JA98 Eq.A2
  double get_mean_Phi () const { return mean_Phi; }
  //! Set the expectation value of Phi
  void set_mean_Phi (double mean_Phi);

  //! Get the variance of Phi
  double get_var_Phi () const { return var_Phi; }

  //! Get the probability distribution of Phi, JA98 Eq.A6
  void get_prob_Phi (unsigned L, std::vector<float>& prob_Phi);

  //! Plots measured and theoretical probability distributions
  class Plot;

 protected:

  //! Set the inverse width of the gaussian, sort of
  void set_alpha (double alpha, double Phi);

  //! The sampling threshold
  double threshold;

  double lo;
  double hi;
  double A;
  double mean_Phi;
  double var_Phi;

};

#endif
