//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/BinaryRule.h,v $
   $Revision: 1.4 $
   $Date: 2005/04/06 20:14:05 $
   $Author: straten $ */

#ifndef __MEAL_BinaryRule_H
#define __MEAL_BinaryRule_H

#include "MEAL/Projection.h"
#include "MEAL/Composite.h"

namespace MEAL {

  //! Abstract base class of binary operators
  template<class T>
  class BinaryRule : public T
  {

  public:

    //! Default constructor
    BinaryRule () : composite(this)  { }

    //! Copy constructor
    BinaryRule (const BinaryRule& rule) : composite(this) { operator=(rule); }

    //! Assignment operator
    BinaryRule& operator = (const BinaryRule& rule);

    //! Destructor
    ~BinaryRule () { }

    //! Set the first argument to the binary operation
    void set_arg1 (T* model);

    //! Set the second argument to the binary operation
    void set_arg2 (T* model);

  protected:

    //! The first argument to the binary operation
    Project<T> arg1;

    //! The first argument to the binary operation
    Project<T> arg2;

  private:

    //! Composite parameter policy
    Composite composite;

  };

}

template<class T>
MEAL::BinaryRule<T>&
MEAL::BinaryRule<T>::operator = (const BinaryRule& rule)
{
  if (this == &rule)
    return *this;

  set_arg1 (rule.arg1);
  set_arg2 (rule.arg2);

  return *this;
}


template<class T>
void MEAL::BinaryRule<T>::set_arg1 (T* model)
{
  if (arg1)
    composite.unmap (arg1, false);

  arg1 = model;

  if (!model)
    return;

  if (verbose)
    std::cerr << "MEAL::BinaryRule::set_model map new model" << std::endl;

  composite.map (arg1);
}

template<class T>
void MEAL::BinaryRule<T>::set_arg2 (T* model)
{
  if (arg2)
    composite.unmap (arg2, false);

  arg2 = model;

  if (!model)
    return;

  if (verbose)
    std::cerr << "MEAL::BinaryRule::set_model map new model" << std::endl;

  composite.map (arg2);
}


#endif
