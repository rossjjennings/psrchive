//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ProductRule.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 19:26:04 $
   $Author: straten $ */

#ifndef __MEAL_ProductRule_H
#define __MEAL_ProductRule_H

#include "MEAL/GroupRule.h"

namespace MEAL {

  //! Represents a product of models
  template<class MType>
  class ProductRule : public GroupRule<MType>
  {

  public:

    typedef typename MType::Result Result;

    //! Add the model to the product
    void operator *= (MType* model) { add_model (model); }

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const
    { return "ProductRule<" + std::string(MType::Name)+ ">"; }

  protected:
    
    // ///////////////////////////////////////////////////////////////////
    //
    // GroupRule implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the identity of the multiplication group (one)
    const Result get_identity() const { return 1.0; }

    //! Multiply the total by the element
    void operate (Result& total, const Result& element) { total *= element; }

    //! Neighbouring terms stay in each other's partial derivatives
    const Result partial (const Result& element) const { return element; }

  };
  
}





#endif

