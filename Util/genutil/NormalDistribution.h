//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/NormalDistribution.h,v $
   $Revision: 1.1 $
   $Date: 2007/10/26 11:43:13 $
   $Author: straten $ */

#ifndef __NormalDistribution_h
#define __NormalDistribution_h

#include "Probability.h"

//! Base class of probability density functions
class NormalDistribution : public Probability {

 public:

  //! Get the value of the probability density at x
  double density (double x);

  //! Get the value of the distribution function at x
  /*! The distribution, \f$ D(x) = \int_{-\inf}^x P(t) dt \f$ */
  double cumulative_distribution (double x);

  //! Get the first moment of the distribution averaged from 0 to x
  /*! i.e. \f$ \int_0^x t P(t) dt \f$ */
  double cumulative_mean (double x);

  //! Get the second moment of the distribution averaged from 0 to x
  /*! i.e. \f$ \int_0^x t^2 P(t) dt \f$ */
  double cumulative_moment2 (double x);


};

#endif

