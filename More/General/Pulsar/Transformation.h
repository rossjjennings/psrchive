//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Transformation.h,v $
   $Revision: 1.4 $
   $Date: 2006/03/17 13:34:52 $
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
