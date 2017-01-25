//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/SumRule.h

#ifndef __SumRule_H
#define __SumRule_H

#include "MEAL/GroupRule.h"

namespace MEAL {

  //! Represents a sum of models
  template<class T>
  class SumRule : public GroupRule<T>
  {

  public:

    typedef typename T::Result Result;

    //! Add the model to the sum
    void operator += (T* model) { this->add_model (model); }

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const
    { return "SumRule<" + std::string(T::Name)+ ">"; }

  protected:
    
    // ///////////////////////////////////////////////////////////////////
    //
    // GroupRule implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the identity of the addition group (zero)
    const Result get_identity () const { return 0.0; }

    //! Add the element to the total
    void operate (Result& total, const Result& element) { total += element; }

    //! Neighbouring terms do not affect each other's partial derivatives
    const Result partial (const Result& element) const { return 0; }

  };
  
  template<class T>
  SumRule<T>* sum (T* A, T* B)
  {
    Reference::To< SumRule<T> > result = new SumRule<T>;
    result->add_model (A);
    result->add_model (B);
    return result.release();
  }

  template<class T>
  SumRule<T>* sum (Reference::To<T>& A, T* B)
  { return sum (A.get(), B); }

  template<class T>
  SumRule<T>* sum (T* A, Reference::To<T>& B)
  { return sum (A, B.get()); }

  template<class T>
  SumRule<T>* sum (Reference::To<T>& A, Reference::To<T>& B)
  { return sum (A.get(), B.get()); }
}


#endif

