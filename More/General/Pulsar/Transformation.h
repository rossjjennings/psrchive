//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/Transformation.h

#ifndef __Pulsar_Transformation_h
#define __Pulsar_Transformation_h

#include "Pulsar/Algorithm.h"

namespace Pulsar {

  //! Algorithms that modify data in the Container
  template<class Container> class Transformation : public Algorithm
  {
  public:

    //! Defined by derived classes
    virtual void transform (Container*) = 0;

    //! Functor interface
    void operator () (Container* container) { transform (container); }

  };

}

#endif
