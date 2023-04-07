//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 - 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/ComplexRVM.h

#ifndef __ComplexRVM_H
#define __ComplexRVM_H

#include "MEAL/Complex.h"
#include "MEAL/ProductRule.h"

namespace MEAL {

  template<typename> class VectorRule;
  
  class RVM;
  class Scalar;

  //! Rotating Vector Model of Stokes Q and U as a function of pulse phase
  class ComplexRVM : public ProductRule<Complex>
  {

  public:

    //! Default constructor
    ComplexRVM ();

    //! Copy constructor
    ComplexRVM (const ComplexRVM& copy);

    //! Assignment operator
    ComplexRVM& operator = (const ComplexRVM& copy);

    //! Destructor
    ~ComplexRVM ();

    //! Return the rotating vector model
    RVM* get_rvm ();
    void set_rvm (RVM*);
    
    //! Add a state: phase in radians, L is measured linear polarization
    void add_state (double phase, std::complex< Estimate<double> >& L);
    //! Set the current state for which the model will be evaluated
    void set_state (unsigned i);

    //! Get the number of states
    unsigned get_nstate () const;

    //! Set the phase of the ith state
    void set_phase (unsigned i, double phase);
    //! Get the phase of the ith state
    double get_phase (unsigned i) const;

    //! Set the linear polarization of the ith state
    void set_linear (unsigned i, const Estimate<double>& L);
    //! Get the linear polarization of the ith state
    Estimate<double> get_linear (unsigned i) const;

#if 0
    //! Correct gain of each state to account for amplitude of complex phase
    void renormalize (double renorm);
#endif

    //! Set the fit flag of every gain parameter
    void set_gains_infit (bool flag=true);

    //! Replace gains with maximum likelihood estimators
    void set_gains_maximum_likelihood (bool flag=true);
    bool get_gains_maximum_likelihood () const { return gains_maximum_likelihood; }

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  private:

    Reference::To<RVM> rvm;

    Reference::To<Scalar> modelQ;
    Reference::To<Scalar> modelU;

    //! Free parameters for gains used when gains_maximum_likelihood is false
    Reference::To< VectorRule<Complex> > gain;

    //! Model each gain with its maximum likelihood estimate
    bool gains_maximum_likelihood;

    class State;
    std::vector<State> state;

    class MaximumLikelihoodGain;

    void check (unsigned i, const char* method) const;
    void init ();
  };

}

#endif
