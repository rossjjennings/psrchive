//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/UnaryRule.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:17 $
   $Author: straten $ */

#ifndef __Calibration_UnaryRule_H
#define __Calibration_UnaryRule_H

#include "Calibration/OptimizedModel.h"
#include "Calibration/Composite.h"

namespace Calibration {

  //! Abstract base class of unary operators
  template<class MType>
  class UnaryRule : public OptimizedModel<MType>, public Composite
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

    //! Set the Model on which the operation will be performed
    void set_model (MType* model);

  protected:

    //! The Model on which the operation will be performed
    Project<MType> model;

  };

}

template<class MType>
Calibration::UnaryRule<MType>&
Calibration::UnaryRule<MType>::operator = (const UnaryRule& rule)
{
  if (this != &rule)
    set_model (rule.model);

  return *this;
}


template<class MType>
void Calibration::UnaryRule<MType>::set_model (MType* _model)
{
  if (model)
    unmap (model, false);

  model = _model;

  if (!_model)
    return;

  if (verbose)
    cerr << "Calibration::UnaryRule::set_model map " 
         << model->get_name() << endl;

  map (model);
}


#endif
