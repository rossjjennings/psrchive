//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Attic/Calibrator.h,v $
   $Revision: 1.2 $
   $Date: 2003/02/18 10:51:10 $
   $Author: pulsar $ */

#ifndef __Calibrator_H
#define __Calibrator_H

#include <string>

#include "Reference.h"

namespace Pulsar {

  //! Forward declaration
  class Archive;

  //! Pure virtual base class of Pulsar::Archive calibrators
  class Calibrator : public Reference::Able {
    
  public:
    //! Verbosity flag
    static bool verbose;

    //! Null constructor
    Calibrator () {}
    
    //! Destructor
    virtual ~Calibrator ();

    //! Calibrate the Pulsar::Archive
    virtual void calibrate (Archive* archive) = 0;

  };

}

#endif
