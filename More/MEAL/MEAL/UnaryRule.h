//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/UnaryRule.h,v $
   $Revision: 1.2 $
   $Date: 2004/11/22 16:00:09 $
   $Author: straten $ */

#ifndef __Model_UnaryRule_H
#define __Model_UnaryRule_H

#include "MEPL/Optimized.h"
#include "MEPL/Composite.h"

namespace Model {

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
Model::UnaryRule<MType>&
Model::UnaryRule<MType>::operator = (const UnaryRule& rule)
{
  if (this != &rule)
    set_model (rule.model);

  return *this;
}


template<class MType>
void Model::UnaryRule<MType>::set_model (MType* _model)
{
  if (model)
    unmap (model, false);

  model = _model;

  if (!_model)
    return;

  if (verbose)
    cerr << "Model::UnaryRule::set_model map " 
         << model->get_name() << endl;

  map (model);
}


#endif
