//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Transformation.h,v $
   $Revision: 1.1 $
   $Date: 2005/12/08 11:10:04 $
   $Author: straten $ */

#ifndef __Pulsar_Algorithm_h
#define __Pulsar_Algorithm_h

namespace Pulsar {

  //! Template base class of algorithms that modify the data in the container
  template<class Container> class Transformation
  {
  public:
    virtual void transform (Container*) = 0;
  };

}

#endif
