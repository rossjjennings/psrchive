//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/RuleMath.h,v $
   $Revision: 1.2 $
   $Date: 2004/11/22 16:00:09 $
   $Author: straten $ */

#ifndef __Model_RuleMath_H
#define __Model_RuleMath_H

#include "MEPL/SumRule.h"
#include "MEPL/NegationRule.h"
#include "MEPL/ProductRule.h"
#include "MEPL/InverseRule.h"

namespace Model {

  template<class Type> SumRule<Type>* Sum (Type* a, Type* b)
  {
    SumRule<Type>* sum = new SumRule<Type>;
    *sum += a;
    *sum += b;
    
    return sum;
  }
  
  template<class Type> SumRule<Type>* Difference (Type* a, Type* b)
  {
    NegationRule<Type>* neg = new NegationRule<Type>;
    neg->set_model (b);
    
    SumRule<Type>* sum = new SumRule<Type>;
    *sum += a;
    *sum += neg;
    
    return sum;
  }

  template<class Type> NegationRule<Type>* Negation (Type* a)
  {
    NegationRule<Type>* neg = new NegationRule<Type>;
    neg->set_model (a);
    return neg;
  }
  
  template<class Type> ProductRule<Type>* Product (Type* a, Type* b)
  {
    ProductRule<Type>* product = new ProductRule<Type>;
    *product *= a;
    *product *= b;
    
    return product;
  }
  
  template<class Type> ProductRule<Type>* Quotient (Type* a, Type* b)
  {
    InverseRule<Type>* inverse = new InverseRule<Type>;
    inverse->set_model (b);
    
    ProductRule<Type>* product = new ProductRule<Type>;
    *product *= a;
    *product *= inverse;
    
    return product;
  }
  
}

#endif

