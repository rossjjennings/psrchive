//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Mean.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 22:17:50 $
   $Author: straten $ */

#ifndef __MEAL_Mean_H
#define __MEAL_Mean_H

#include "ReferenceAble.h"

namespace MEAL {

  //! Used to accumulate a running mean of Complex2 model parameters
  /*! This pure virtual template base class defines the interface by
    which mean Function parameters may be accumulated and used. */

  template<class MType>
  class Mean : public Reference::Able {

  public:

    //! Add the Function parameters to the running mean
    virtual void integrate (const MType* model) = 0;

    //! Update the model parameters with the current value of the mean
    virtual void update (MType* model) = 0;

  };

}

#endif

