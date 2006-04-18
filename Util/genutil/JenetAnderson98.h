//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/JenetAnderson98.h,v $
   $Revision: 1.3 $
   $Date: 2006/04/18 14:26:10 $
   $Author: straten $ */

#ifndef __Jenet_Anderson_98
#define __Jenet_Anderson_98

#include <vector>

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
  void set_threshold (double t);

  //! Get the expectation value of Phi, JA98 Eq.A2
  float get_mean_Phi () const;

  //! Get the probability distribution of Phi, JA98 Eq.A6
  void get_prob_Phi (unsigned L, std::vector<float>& prob_Phi);

 protected:

  //! Set the inverse width of the gaussian, sort of
  void set_alpha (double alpha, double Phi);

  //! The sampling threshold
  double threshold;

  double lo;
  double hi;
  double A;
  double Phi;

};

#endif
