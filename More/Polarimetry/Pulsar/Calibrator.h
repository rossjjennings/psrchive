//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Attic/Calibrator.h,v $
   $Revision: 1.5 $
   $Date: 2003/04/27 10:53:20 $
   $Author: straten $ */

#ifndef __Calibrator_H
#define __Calibrator_H

#include <vector>

#include "Reference.h"

template<typename T> class Jones;

namespace Pulsar {

  //! Forward declarations
  class Archive;
  class Integration;

  //! Pure virtual base class of Pulsar::Archive calibrators
  class Calibrator : public Reference::Able {
    
  public:
    //! Verbosity flag
    static bool verbose;

    //! Fractional bandwidth of the window used in median filter
    static float median_smoothing;

    //! Null constructor
    Calibrator () {}
    
    //! Destructor
    virtual ~Calibrator ();

    //! Calibrate the Pulsar::Archive
    virtual void calibrate (Archive* archive) = 0;

  protected:

    //! Provide access to Integration::transform
    virtual void calibrate (Integration* integration,
			    const vector< Jones<float> >& response);

  };

}

#endif

