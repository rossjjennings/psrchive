//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/UnaryRule.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 19:26:04 $
   $Author: straten $ */

#ifndef __MEAL_UnaryRule_H
#define __MEAL_UnaryRule_H

#include "MEAL/Optimized.h"
#include "MEAL/Composite.h"

namespace MEAL {

  //! Abstract base class of unary operators
  template<class MType>
  class UnaryRule : public Optimized<MType>, public Composite
  {

  public:

    //! Default constructor
    UnaryRule () { }

    //! Copy constructor
    UnaryRule (const UnaryRule& rule) { operator = (rule); }

    //! Assignment operator
    UnaryRule& operator = (const UnaryRule& rule);

    //! Destructor
    ~UnaryRule () { }

    //! Set the Function on which the operation will be performed
    void set_model (MType* model);

  protected:

    //! The Function on which the operation will be performed
    Project<MType> model;

  };

}

template<class MType>
MEAL::UnaryRule<MType>&
MEAL::UnaryRule<MType>::operator = (const UnaryRule& rule)
{
  if (this != &rule)
    set_model (rule.model);

  return *this;
}


template<class MType>
void MEAL::UnaryRule<MType>::set_model (MType* _model)
{
  if (model)
    unmap (model, false);

  model = _model;

  if (!_model)
    return;

  if (verbose)
    std::cerr << "MEAL::UnaryRule::set_model map " 
         << model->get_name() << std::endl;

  map (model);
}


#endif
