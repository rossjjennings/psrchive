//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/SumRule.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:17 $
   $Author: straten $ */

#ifndef __SumRule_H
#define __SumRule_H

#include "Calibration/GroupRule.h"

namespace Calibration {

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
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    string get_name () const
    { return "SumRule<" + string(MType::Name)+ ">"; }

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

