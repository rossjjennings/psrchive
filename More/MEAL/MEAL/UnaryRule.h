//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/UnaryRule.h,v $
   $Revision: 1.4 $
   $Date: 2005/04/06 20:14:05 $
   $Author: straten $ */

#ifndef __MEAL_UnaryRule_H
#define __MEAL_UnaryRule_H

#include "MEAL/Projection.h"
#include "MEAL/Composite.h"

namespace MEAL {

  //! Abstract base class of unary operators
  template<class T>
  class UnaryRule : public T
  {

  public:

    //! Default constructor
    UnaryRule () : composite(this) { }

    //! Copy constructor
    UnaryRule (const UnaryRule& rule) : composite(this) { operator = (rule); }

    //! Assignment operator
    UnaryRule& operator = (const UnaryRule& rule);

    //! Destructor
    ~UnaryRule () { }

    //! Set the Function on which the operation will be performed
    void set_model (T* model);

  protected:

    //! The Function on which the operation will be performed
    Project<T> model;

  private:

    //! Composite parameter policy
    Composite composite;

  };

}

template<class T>
MEAL::UnaryRule<T>&
MEAL::UnaryRule<T>::operator = (const UnaryRule& rule)
{
  if (this != &rule)
    set_model (rule.model);

  return *this;
}


template<class T>
void MEAL::UnaryRule<T>::set_model (T* _model)
{
  if (model)
    composite.unmap (model, false);

  model = _model;

  if (!_model)
    return;

  if (verbose)
    std::cerr << "MEAL::UnaryRule::set_model map " 
         << model->get_name() << std::endl;

  composite.map (model);
}


#endif
