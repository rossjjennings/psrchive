//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/BinaryRule.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:14 $
   $Author: straten $ */

#ifndef __Calibration_BinaryRule_H
#define __Calibration_BinaryRule_H

#include "Calibration/OptimizedModel.h"
#include "Calibration/Composite.h"

namespace Calibration {

  //! Abstract base class of binary operators
  template<class MType>
  class BinaryRule : public OptimizedModel<MType>, public Composite
  {

  public:

    //! Default constructor
    BinaryRule () { }

    //! Copy constructor
    BinaryRule (const BinaryRule& rule) { operator = (rule); }

    //! Assignment operator
    BinaryRule& operator = (const BinaryRule& rule);

    //! Destructor
    ~BinaryRule () { }

    //! Set the first argument to the binary operation
    void set_arg1 (MType* model);

    //! Set the second argument to the binary operation
    void set_arg2 (MType* model);

  protected:

    //! The first argument to the binary operation
    Project<MType> arg1;

    //! The first argument to the binary operation
    Project<MType> arg2;

  };

}

template<class MType>
Calibration::BinaryRule<MType>&
Calibration::BinaryRule<MType>::operator = (const BinaryRule& rule)
{
  if (this == &rule)
    return *this;

  set_arg1 (rule.arg1);
  set_arg2 (rule.arg2);

  return *this;
}


template<class MType>
void Calibration::BinaryRule<MType>::set_arg1 (MType* model)
{
  if (arg1)
    unmap (arg1, false);

  arg1 = model;

  if (!model)
    return;

  if (verbose)
    cerr << "Calibration::BinaryRule::set_model map new model" << endl;

  map (arg1);
}

template<class MType>
void Calibration::BinaryRule<MType>::set_arg2 (MType* model)
{
  if (arg2)
    unmap (arg2, false);

  arg2 = model;

  if (!model)
    return;

  if (verbose)
    cerr << "Calibration::BinaryRule::set_model map new model" << endl;

  map (arg2);
}


#endif
