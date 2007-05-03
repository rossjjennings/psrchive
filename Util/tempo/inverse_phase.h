//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/inverse_phase.h,v $
   $Revision: 1.1 $
   $Date: 2007/05/03 21:04:09 $
   $Author: straten $ */

#ifndef __Predictor_h
#define __Predictor_h

#include <iostream>

namespace Pulsar {

  //! implements an inverse phase function using Newton-Raphson method

  /*! This template function uses the Newton-Raphson method to solve:

  PHASE = p (TIME)

  for TIME, given PHASE.
  */

  template<typename Predictor>
  MJD inverse_phase (const Predictor& predictor, const Phase& p)
  {
    MJD guess = predictor.get_reftime() + (p - predictor.get_refphase())
      / predictor.get_reffrequency();

    MJD dt;
  
    int gi = 0;
    double converge_faster = 1.0;  // kludge!!
    double converge_factor = 0.5;
  
    double lprecision = max (Predictor::precision, MJD::precision);

    for (gi=0; gi<10000; gi++) {
      dt = (predictor.phase(guess) - p) / predictor.frequency(guess);
      
      guess -= dt * converge_faster;
      
      // every six iterations, give the convergence a little bump
      if (gi && !(gi % 6))
	converge_faster *= converge_factor;
      
      if (fabs (dt.in_seconds()) < lprecision)
	return guess;
    }
    
    std::cerr << "polynomial::iphase maximum iterations exceeded - error="
	      << dt.in_seconds() * 1e6 << "us" << std::endl;
    
    return guess;
  }
 
};

#endif
