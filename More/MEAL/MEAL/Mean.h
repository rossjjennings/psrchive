//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Mean.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 16:00:09 $
   $Author: straten $ */

#ifndef __Model_MeanFunction_H
#define __Model_MeanFunction_H

#include "ReferenceAble.h"

namespace Model {

  //! Used to accumulate a running mean of Complex2 model parameters
  /*! This pure virtual template base class defines the interface by
    which mean Function parameters may be accumulated and used. */

  template<class MType>
  class MeanFunction : public Reference::Able {

  public:

    //! Add the Function parameters to the running mean
    virtual void integrate (const MType* model) = 0;

    //! Update the model parameters with the current value of the mean
    virtual void update (MType* model) = 0;

  };

}

#endif

