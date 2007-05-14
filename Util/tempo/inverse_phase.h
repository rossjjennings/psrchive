//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/inverse_phase.h,v $
   $Revision: 1.4 $
   $Date: 2007/05/14 21:20:33 $
   $Author: straten $ */

#ifndef __Predictor_h
#define __Predictor_h

#include "MJD.h"
#include "Phase.h"

#include <iostream>

namespace Pulsar {

  //! implements an inverse phase function using the Newton-Raphson method

  /*! This template function uses the Newton-Raphson method to solve:

  PHASE = p (TIME)

  for TIME, given PHASE.

  This function requires Predictor to implement the following interface:

  class Predictor {
  public:
    static double precision;

    MJD get_reftime() const;
    Phase get_refphase() const;
    long double get_reffrequency() const;
    Phase phase (const MJD&) const;
    long double frequency (const MJD&) const;
  };

  */

  //! Number of times inverse_phase is called
  extern unsigned inverse_phase_calls;
  //! Total number of iterations
  extern unsigned inverse_phase_iterations;
  //! Inverse phase verbosity
  extern bool inverse_phase_verbose;

  template<typename Predictor>
  MJD inverse_phase (const Predictor& predictor,
		     const Phase& p, const MJD* first_guess = 0)
  {
    MJD guess;

    if (first_guess)  {
      guess = *first_guess;
      if (inverse_phase_verbose)
        std::cerr << "inverse_phase: given guess = " << guess << std::endl;
    } 
    else  {
      guess = predictor.get_reftime()
	+ (p - predictor.get_refphase()) / predictor.get_reffrequency();
      if (inverse_phase_verbose)
	std::cerr << "inverse_phase: first guess = " << guess << std::endl;
    }

    MJD dt;
    int gi = 0;

#if 0
    double converge_faster = 1.0;  // kludge!!
    double converge_factor = 0.5;
#endif

    double lprecision = std::max (Predictor::precision, MJD::precision);
    if (inverse_phase_verbose)
      std::cerr << "inverse_phase: precision=" << lprecision * 1e6 << " us" << std::endl;

    inverse_phase_calls ++;

    for (gi=0; gi<10000; gi++) {

      inverse_phase_iterations ++;

      dt = (predictor.phase(guess) - p) / predictor.frequency(guess);
      
      guess -= dt; // * converge_faster;

      if (inverse_phase_verbose)
        std::cerr << "inverse_phase: guess=" << guess.printdays(20)
		  << " dt=" << dt.in_seconds()*1e6 << " us" << std::endl;

#if 0
      // every six iterations, give the convergence a little bump
      if (gi && !(gi % 6))
	converge_faster *= converge_factor;
#endif
      
      if (fabs (dt.in_seconds()) < lprecision)
	return guess;
    }
    
    std::cerr << "polynomial::iphase maximum iterations exceeded - error="
	      << dt.in_seconds() * 1e6 << "us" << std::endl;
    
    return guess;
  }
 
};

#endif
