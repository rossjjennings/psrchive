//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Transformation.h,v $
   $Revision: 1.3 $
   $Date: 2005/12/09 16:41:07 $
   $Author: straten $ */

#ifndef __Pulsar_Transformation_h
#define __Pulsar_Transformation_h

#include "Pulsar/Algorithm.h"

namespace Pulsar {

  //! Algorithms that modify data in the Container
  template<class Container> class Transformation : public Algorithm
  {
  public:
    virtual void transform (Container*) = 0;
  };

}

#endif
