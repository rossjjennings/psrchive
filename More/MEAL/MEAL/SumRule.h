//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/SumRule.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 19:26:04 $
   $Author: straten $ */

#ifndef __SumRule_H
#define __SumRule_H

#include "MEAL/GroupRule.h"

namespace MEAL {

  //! Represents a sum of models
  template<class MType>
  class SumRule : public GroupRule<MType>
  {

  public:

    typedef typename MType::Result Result;

    //! Add the model to the sum
    void operator += (MType* model) { add_model (model); }

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const
    { return "SumRule<" + std::string(MType::Name)+ ">"; }

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
  
}


#endif

