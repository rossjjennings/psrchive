//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Transformation.h,v $
   $Revision: 1.2 $
   $Date: 2005/12/09 03:56:02 $
   $Author: straten $ */

#ifndef __Pulsar_Transformation_h
#define __Pulsar_Transformation_h

#include "Pulsar/Algorithm.h"

namespace Pulsar {

  //! Template base class of algorithms that modify the data in the container
  template<class Container> class Transformation : public Algorithm
  {
  public:
    virtual void transform (Container*) = 0;
  };

}

#endif
