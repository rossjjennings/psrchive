//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ProductRule.h,v $
   $Revision: 1.2 $
   $Date: 2004/11/22 16:00:09 $
   $Author: straten $ */

#ifndef __Model_ProductRule_H
#define __Model_ProductRule_H

#include "MEPL/GroupRule.h"

namespace Model {

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
    string get_name () const
    { return "ProductRule<" + string(MType::Name)+ ">"; }

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

